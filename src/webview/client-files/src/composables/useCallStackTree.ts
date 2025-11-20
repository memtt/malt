/**
 * Composable for Call Stack Tree Management
 * Uses a FLAT array structure for better Vue reactivity
 */

import { ref } from 'vue';
import { fetchCallStackLevel } from '@/services/sourcesService';
import type { CallStackEntry, CallStackFilter } from '@/types/sources';

export interface CallStackNode extends CallStackEntry {
  expanded?: boolean;
  loading?: boolean;
  depth: number;
}

export function useCallStackTree() {
  // State - flat array of visible nodes
  const flatNodes = ref<CallStackNode[]>([]);
  const loading = ref(false);
  const error = ref<string | null>(null);

  // AsyncState counter to avoid race conditions
  const asyncState = ref(0);

  /**
   * Load root level or children of a node
   */
  const loadLevel = async (
    parentStackId: number,
    parentStackDepth: number,
    filter: CallStackFilter,
    currentAsyncState: number,
  ): Promise<CallStackEntry[]> => {
    try {
      const entries = await fetchCallStackLevel(
        parentStackId,
        parentStackDepth,
        filter,
      );

      // Check if this request is still valid
      if (currentAsyncState !== asyncState.value) {
        return [];
      }

      return entries;
    } catch (err) {
      console.error('Failed to load call stack level:', err);
      error.value = err instanceof Error ? err.message : 'Failed to load';
      return [];
    }
  };

  /**
   * Update tree with new filter (file/line or function)
   */
  const updateByFilter = async (filter: CallStackFilter): Promise<void> => {
    asyncState.value++;
    const currentState = asyncState.value;

    loading.value = true;
    error.value = null;

    try {
      const entries = await loadLevel(0, 0, filter, currentState);

      if (currentState === asyncState.value) {
        // Create flat nodes with depth 0
        flatNodes.value = entries.map(entry => ({
          ...entry,
          expanded: false,
          loading: false,
          depth: 0,
        }));
      }
    } catch (err) {
      if (currentState === asyncState.value) {
        error.value = err instanceof Error ? err.message : 'Failed to load';
      }
    } finally {
      if (currentState === asyncState.value) {
        loading.value = false;
      }
    }
  };

  /**
   * Update tree by file and line
   */
  const updateByLocation = async (
    file: string,
    line: number,
  ): Promise<void> => {
    await updateByFilter({ file, line });
  };

  /**
   * Update tree by function name
   */
  const updateByFunction = async (func: string): Promise<void> => {
    await updateByFilter({ function: func });
  };

  /**
   * Expand a node (load and insert its children)
   */
  const expandNode = async (node: CallStackNode): Promise<void> => {
    if (node.expanded || node.loading || !node.hasChild) {
      return;
    }

    // Set loading
    node.loading = true;

    // Build filter from current node
    const filter: CallStackFilter = {};
    if (node.location.file) filter.file = node.location.file;
    if (node.location.function) filter.function = node.location.function;

    const currentState = asyncState.value;

    const entries = await loadLevel(
      node.stackId,
      node.stackDepth,
      filter,
      currentState,
    );

    if (currentState !== asyncState.value) {
      return;
    }

    // Find node index in flat array
    const nodeIndex = flatNodes.value.findIndex(
      n => n.stackId === node.stackId && n.stackDepth === node.stackDepth,
    );

    if (nodeIndex === -1) {
      return;
    }

    // Mark node as expanded
    node.expanded = true;
    node.loading = false;

    // Create child nodes with depth + 1
    const childNodes: CallStackNode[] = entries.map(entry => ({
      ...entry,
      expanded: false,
      loading: false,
      depth: node.depth + 1,
    }));

    // Insert children right after parent in flat array
    flatNodes.value.splice(nodeIndex + 1, 0, ...childNodes);
  };

  /**
   * Collapse a node (remove its descendants)
   */
  const collapseNode = (node: CallStackNode): void => {
    // Find node index
    const nodeIndex = flatNodes.value.findIndex(
      n => n.stackId === node.stackId && n.stackDepth === node.stackDepth,
    );

    if (nodeIndex === -1) {
      return;
    }

    // Mark as collapsed
    node.expanded = false;

    // Count how many descendants to remove
    // (all nodes with depth > node.depth until we hit a node with depth <= node.depth)
    let removeCount = 0;
    for (let i = nodeIndex + 1; i < flatNodes.value.length; i++) {
      if (flatNodes.value[i].depth <= node.depth) {
        break;
      }
      removeCount++;
    }

    // Remove descendants
    if (removeCount > 0) {
      flatNodes.value.splice(nodeIndex + 1, removeCount);
    }
  };

  /**
   * Toggle node expansion
   */
  const toggleNode = async (node: CallStackNode): Promise<void> => {
    if (node.expanded) {
      collapseNode(node);
    } else {
      await expandNode(node);
    }
  };

  /**
   * Clear the entire tree
   */
  const clear = (): void => {
    flatNodes.value = [];
    error.value = null;
  };

  return {
    // State
    flatNodes,
    loading,
    error,

    // Methods
    updateByLocation,
    updateByFunction,
    expandNode,
    collapseNode,
    toggleNode,
    clear,
  };
}

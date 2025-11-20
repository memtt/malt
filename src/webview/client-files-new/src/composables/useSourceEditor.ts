/**
 * Composable for Source Code Editor State Management
 * Handles file loading, annotations, and navigation
 * Ported from Angular MaltSourceEditor class
 */

import { ref, computed } from 'vue';
import {
  fetchSourceFile,
  fetchFileAnnotations,
} from '@/services/sourcesService';
import { enrichAnnotations, findLargestAnnotationLine } from '@/utils/sources';
import type { SourceAnnotation } from '@/types/sources';

export interface PostMoveAction {
  type: 'line' | 'func';
  line?: number;
  func?: string;
}

export interface UseSourceEditorOptions {
  /**
   * Function to get value from annotation for finding largest line
   */
  getValue?: (annotation: SourceAnnotation) => number;
}

export function useSourceEditor(options: UseSourceEditorOptions = {}) {
  // State
  const currentFile = ref<string | null>(null);
  const currentFunction = ref<string>('');
  const sourceCode = ref<string>('');
  const annotations = ref<SourceAnnotation[]>([]);
  const selectedLine = ref<number | null>(null);
  const loading = ref(false);
  const error = ref<string | null>(null);
  const postMoveAction = ref<PostMoveAction | null>(null);

  /**
   * Check if source file is available
   */
  const hasSource = computed(() => {
    return !(
      currentFile.value === null ||
      currentFile.value === 'No source file...' ||
      currentFile.value === '??' ||
      currentFile.value === '' ||
      currentFile.value === undefined
    );
  });

  /**
   * Load source file content
   */
  const loadSourceFile = async (filePath: string): Promise<void> => {
    // Skip if same file
    if (currentFile.value === filePath) {
      const targetLine = executePostMoveAction();
      if (targetLine !== null) {
        selectedLine.value = targetLine;
      }
      return;
    }

    // Skip invalid files
    if (!filePath || filePath === '??' || filePath === '') {
      currentFile.value = filePath;
      sourceCode.value = '';
      error.value = 'No source file available';
      return;
    }

    loading.value = true;
    error.value = null;

    try {
      const code = await fetchSourceFile(filePath);
      sourceCode.value = code;
      currentFile.value = filePath;

      // Load annotations for this file
      await loadAnnotations(filePath);
    } catch (err) {
      error.value = err instanceof Error ? err.message : 'Failed to load file';
      sourceCode.value = `Source for the file, ${filePath}, could not be loaded.`;
    } finally {
      loading.value = false;

      // Execute post-move action AFTER loading is complete
      // This ensures the DOM is ready before scrolling
      const targetLine = executePostMoveAction();
      if (targetLine !== null) {
        selectedLine.value = targetLine;
      }
    }
  };

  /**
   * Load annotations for a file
   */
  const loadAnnotations = async (filePath: string): Promise<void> => {
    try {
      const rawAnnotations = await fetchFileAnnotations(filePath);
      annotations.value = enrichAnnotations(rawAnnotations, filePath);
    } catch (err) {
      console.error('Failed to load annotations:', err);
      annotations.value = [];
    }
  };

  /**
   * Move to a specific file
   */
  const moveToFile = async (filePath: string): Promise<void> => {
    postMoveAction.value = null;
    await loadSourceFile(filePath);
  };

  /**
   * Move to a specific file and function
   * Will scroll to the line with the largest annotation for that function
   */
  const moveToFileFunction = async (
    filePath: string,
    func: string,
  ): Promise<void> => {
    currentFunction.value = func;

    if (func && func !== '' && func !== '??') {
      postMoveAction.value = { type: 'func', func };
    } else {
      postMoveAction.value = null;
    }

    await loadSourceFile(filePath);
  };

  /**
   * Move to a specific file and line
   */
  const moveToFileLine = async (
    filePath: string,
    line: number,
  ): Promise<void> => {
    postMoveAction.value = { type: 'line', line };
    await loadSourceFile(filePath);
  };

  /**
   * Execute post-move action (scroll to line or function)
   * This should be called after the editor DOM is ready
   */
  const executePostMoveAction = (): number | null => {
    if (!postMoveAction.value) return null;

    if (postMoveAction.value.type === 'line' && postMoveAction.value.line) {
      selectedLine.value = postMoveAction.value.line;
      return postMoveAction.value.line;
    }

    if (postMoveAction.value.type === 'func' && postMoveAction.value.func) {
      // Find line with largest annotation for this function
      const getValue = options.getValue || (() => 0);
      const line = findLargestAnnotationLine(
        annotations.value,
        currentFile.value || '',
        postMoveAction.value.func,
        getValue,
      );

      if (line !== -1) {
        selectedLine.value = line;
        return line;
      }
    }

    return null;
  };

  /**
   * Get annotation for a specific line
   */
  const getAnnotationForLine = (
    line: number,
  ): SourceAnnotation | undefined => {
    return annotations.value.find((ann) => ann.line === line);
  };

  /**
   * Select a line (e.g., when clicking on an annotation)
   */
  const selectLine = (line: number): void => {
    selectedLine.value = line;
  };

  /**
   * Clear selection
   */
  const clearSelection = (): void => {
    selectedLine.value = null;
  };

  return {
    // State
    currentFile,
    currentFunction,
    sourceCode,
    annotations,
    selectedLine,
    loading,
    error,
    hasSource,

    // Methods
    loadSourceFile,
    loadAnnotations,
    moveToFile,
    moveToFileFunction,
    moveToFileLine,
    executePostMoveAction,
    getAnnotationForLine,
    selectLine,
    clearSelection,
  };
}

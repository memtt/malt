/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/composables/useCallTreeNavigation.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
/**
 * Composable for Call Tree Navigation History
 * Manages back/forward navigation through call tree nodes
 */

import { ref, computed } from 'vue';
import type { CallTreeNavigationState } from '@/types/call-tree';

export function useCallTreeNavigation() {
  // Navigation history stacks
  const history = ref<CallTreeNavigationState[]>([]);
  const forwardHistory = ref<CallTreeNavigationState[]>([]);

  /**
   * Current navigation state
   */
  const currentState = computed<CallTreeNavigationState | null>(
    () => history.value[history.value.length - 1] || null,
  );

  /**
   * Can navigate back
   */
  const canGoBack = computed(() => history.value.length > 1);

  /**
   * Can navigate forward
   */
  const canGoForward = computed(() => forwardHistory.value.length > 0);

  /**
   * Navigate to a new node
   * Clears forward history
   */
  const navigateTo = (state: CallTreeNavigationState) => {
    history.value.push(state);
    forwardHistory.value = [];
  };

  /**
   * Update the current navigation state without adding to history
   * Useful for updating nodeId after backend response
   */
  const updateCurrentState = (state: CallTreeNavigationState) => {
    if (history.value.length > 0) {
      history.value[history.value.length - 1] = state;
    } else {
      history.value.push(state);
    }
  };

  /**
   * Navigate back in history
   * @returns Previous state, or null if can't go back
   */
  const goBack = (): CallTreeNavigationState | null => {
    if (!canGoBack.value) return null;

    const current = history.value.pop()!;
    forwardHistory.value.push(current);

    return history.value[history.value.length - 1];
  };

  /**
   * Navigate forward in history
   * @returns Next state, or null if can't go forward
   */
  const goForward = (): CallTreeNavigationState | null => {
    if (!canGoForward.value) return null;

    const next = forwardHistory.value.pop()!;
    history.value.push(next);

    return next;
  };

  /**
   * Clear all navigation history
   */
  const clearHistory = () => {
    history.value = [];
    forwardHistory.value = [];
  };

  /**
   * Reset to initial state (root node)
   */
  const resetToRoot = () => {
    clearHistory();
    navigateTo({ nodeId: -1 }); // -1 means root node (not searching by function)
  };

  return {
    // State
    history: computed(() => history.value),
    forwardHistory: computed(() => forwardHistory.value),
    currentState,

    // Computed
    canGoBack,
    canGoForward,

    // Actions
    navigateTo,
    updateCurrentState,
    goBack,
    goForward,
    clearHistory,
    resetToRoot,
  };
}

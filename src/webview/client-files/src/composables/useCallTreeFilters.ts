/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/composables/useCallTreeFilters.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
/**
 * Composable for Call Tree Filters Management
 * Manages height, depth, and node cost filters
 */

import { ref, computed } from 'vue';
import type { CallTreeFilters } from '@/types/call-tree';

/**
 * Available filter options
 */
export const HEIGHT_OPTIONS = [-1, 1, 2, 3, 4, 5] as const;
export const DEPTH_OPTIONS = [-1, 1, 2, 3, 4, 5] as const;
export const NODE_COST_OPTIONS = [0, 1, 2, 3, 5, 10, 15, 20, 30, 50] as const;

/**
 * Filter option labels
 */
export function getFilterLabel(value: number): string {
  if (value === -1) return 'Unlimited';
  return String(value);
}

export function getNodeCostLabel(value: number): string {
  if (value === 0) return 'None';
  return `${value}%`;
}

export function useCallTreeFilters() {
  // Default values match Angular version
  const height = ref<number>(-1);
  const depth = ref<number>(3);
  const nodeCost = ref<number>(1);

  const filters = computed<CallTreeFilters>(() => ({
    height: height.value,
    depth: depth.value,
    nodeCost: nodeCost.value,
  }));

  /**
   * Reset filters to default values
   */
  const resetFilters = () => {
    height.value = -1;
    depth.value = 3;
    nodeCost.value = 1;
  };

  /**
   * Set all filters at once
   */
  const setFilters = (newFilters: Partial<CallTreeFilters>) => {
    if (newFilters.height !== undefined) height.value = newFilters.height;
    if (newFilters.depth !== undefined) depth.value = newFilters.depth;
    if (newFilters.nodeCost !== undefined) nodeCost.value = newFilters.nodeCost;
  };

  return {
    // State
    height,
    depth,
    nodeCost,
    filters,

    // Actions
    resetFilters,
    setFilters,

    // Options
    heightOptions: HEIGHT_OPTIONS,
    depthOptions: DEPTH_OPTIONS,
    nodeCostOptions: NODE_COST_OPTIONS,
  };
}

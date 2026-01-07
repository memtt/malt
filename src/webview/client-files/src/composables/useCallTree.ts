/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/composables/useCallTree.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/
/**
 * Main Call Tree Composable
 * Combines data fetching, filters, and navigation
 */

import { computed, toValue, watch, type MaybeRef } from 'vue'
import { useCallTreeData } from '@/composables/useCallTreeData'
import { useCallTreeFilters } from '@/composables/useCallTreeFilters'
import { useCallTreeNavigation } from '@/composables/useCallTreeNavigation'
import type { CallTreeParams } from '@/types/call-tree'
import type { FunctionStat } from '@/types/sources'

interface UseCallTreeOptions {
  /** Metric to use (reactive ref or initial value) */
  metric?: MaybeRef<string>
  /** Ratio mode (reactive ref or initial value) */
  ratio?: MaybeRef<boolean>
}

export function useCallTree(options: UseCallTreeOptions = {}) {
  // Child composables
  const filters = useCallTreeFilters()
  const navigation = useCallTreeNavigation()

  // Metric selection
  const selectedMetric = computed(() => toValue(options.metric) ?? 'peakmem.global')
  const ratio = computed(() => toValue(options.ratio) ?? false)

  /**
   * Build API request parameters
   */
  const requestParams = computed<CallTreeParams>(() => {
    const state = navigation.currentState.value

    return {
      nodeid: state?.nodeId ?? null,
      func: state?.function ?? null,
      depth: filters.depth.value,
      height: filters.height.value,
      mincost: filters.nodeCost.value,
      metric: selectedMetric.value,
      isratio: ratio.value,
    }
  })

  // Data fetching
  const { data, loading, error, refetch } = useCallTreeData({
    params: requestParams,
  })

  /**
   * Load graph by node ID
   * @param nodeId - Node ID to load (-1 for root)
   * @param shouldNavigate - Whether to add to navigation history
   */
  const loadByNodeId = async (nodeId: number, shouldNavigate = true) => {
    if (shouldNavigate) {
      navigation.navigateTo({ nodeId })
    }
    await refetch()
  }

  /**
   * Load graph by function name
   * @param func - Function stat object
   */
  const loadByFunction = async (func: FunctionStat) => {
    navigation.navigateTo({
      nodeId: null, // Let backend find the node by function name
      function: func.function,
    })
    await refetch()

    // Update navigation with actual nodeId from backend response
    if (data.value?.nodeId) {
      navigation.updateCurrentState({
        nodeId: data.value.nodeId,
        function: func.function,
      })
    }
  }

  /**
   * Navigate back in history
   */
  const goBack = async () => {
    const prevState = navigation.goBack()
    if (prevState) {
      await refetch()
    }
  }

  /**
   * Navigate forward in history
   */
  const goForward = async () => {
    const nextState = navigation.goForward()
    if (nextState) {
      await refetch()
    }
  }

  /**
   * Reset to root node
   */
  const resetToRoot = async () => {
    navigation.resetToRoot()
    await refetch()
  }

  /**
   * Auto-refetch when filters or metric change
   */
  watch([selectedMetric, ratio, filters.filters], () => {
    refetch()
  })

  /**
   * Initialize: Navigate to root on first load
   */
  navigation.resetToRoot()

  return {
    // Data
    data,
    loading,
    error,

    // Filters
    filters,

    // Navigation
    navigation,
    canGoBack: navigation.canGoBack,
    canGoForward: navigation.canGoForward,

    // Metric selection
    selectedMetric,
    ratio,

    // Actions
    loadByNodeId,
    loadByFunction,
    goBack,
    goForward,
    resetToRoot,
    refetch,
  }
}

/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/composables/useHome.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
/**
 * Main Home Composable
 * Combines data fetching and summary utilities
 */

import { computed } from 'vue'
import { useHomeData } from '@/composables/useHomeData'
import { useHomeSummary } from '@/composables/useHomeSummary'
import { useFlatData } from '@/composables/useFlatData'
import { useFlatTopFunctions } from '@/composables/useFlatTopFunctions'

export interface MetricDefinition {
  key: string
  label: string
  description: string
}

// Metric definitions for top functions sections
export const metricDefinitions: Record<string, MetricDefinition> = {
  'alloc.count': {
    key: 'alloc.count',
    label: 'Allocation count',
    description:
      'Number of memory allocations (malloc, calloc, realloc) called by this function. This metric shows which functions are responsible for the most allocation calls, regardless of the size allocated.',
  },
  'alloc.sum': {
    key: 'alloc.sum',
    label: 'Cumulated allocations',
    description:
      'Total amount of memory allocated by this function across all calls. This is the sum of all allocation sizes and helps identify functions that allocate the most memory in total.',
  },
  'peakmem.global': {
    key: 'peakmem.global',
    label: 'Memory usage on peak',
    description:
      'Amount of memory still alive from this function when the application reached its peak memory usage. This helps identify which functions contribute the most to the peak memory footprint.',
  },
  leaks: {
    key: 'leaks',
    label: 'Leaks',
    description:
      'Memory allocated by this function that was never freed before the program ended. This indicates memory leaks and helps identify which functions are responsible for leaked memory.',
  },
}

export function useHome() {
  // Fetch summary data
  const { data, loading, error, refetch } = useHomeData()

  // Fetch flat functions data
  const { data: flatData, loading: flatLoading, error: flatError } = useFlatData()

  // Get summary utilities
  const summary = useHomeSummary(data)

  // Convert flatData Ref to ComputedRef for useFlatTopFunctions
  const flatDataComputed = computed(() => flatData.value)

  // Get top functions for different metrics
  const { topFunctions: topAllocCount } = useFlatTopFunctions(flatDataComputed, 'alloc.count', {
    limit: 5,
    inclusive: false,
  })

  const { topFunctions: topAllocSum } = useFlatTopFunctions(flatDataComputed, 'alloc.sum', {
    limit: 5,
    inclusive: false,
  })

  const { topFunctions: topPeakMem } = useFlatTopFunctions(flatDataComputed, 'peakmem.global', {
    limit: 5,
    inclusive: false,
  })

  const { topFunctions: topLeaks } = useFlatTopFunctions(flatDataComputed, 'leaks', {
    limit: 5,
    inclusive: false,
  })

  return {
    // Data state
    data,
    loading,
    error,
    refetch,

    // Flat functions data
    flatData,
    flatLoading,
    flatError,

    // Top functions
    topAllocCount,
    topAllocSum,
    topPeakMem,
    topLeaks,

    // Metrics
    metricDefinitions,

    // Summary utilities
    ...summary,
  }
}

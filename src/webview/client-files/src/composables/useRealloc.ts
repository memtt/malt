/**
 * Composable for Realloc - Composition Layer
 * Combines data, summary, and charts composables
 */

import { useReallocData } from '@/composables/useReallocData'
import { useReallocSummary } from '@/composables/useReallocSummary'
import { useReallocCharts } from '@/composables/useReallocCharts'

export function useRealloc() {
  // Get data
  const { data, loading, error } = useReallocData()

  // Get summary
  const { mostUsed, meanDelta, totalCount, meanDeltaFormatted } = useReallocSummary(data)

  // Get charts
  const { deltaDistribution, log2Histogram, topMostUsed, topDeltas, mostUsedBarData } =
    useReallocCharts(data)

  return {
    // Data state
    data,
    loading,
    error,

    // Summary
    mostUsed,
    meanDelta,
    totalCount,
    meanDeltaFormatted,

    // Chart data
    deltaDistribution,
    log2Histogram,
    topMostUsed,
    topDeltas,
    mostUsedBarData,
  }
}

/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/composables/usePerThread.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
import { computed } from 'vue'
import { usePerThreadData } from '@/composables/usePerThreadData'
import { usePerThreadCharts } from '@/composables/usePerThreadCharts'

/**
 * Main composable for Per-Thread page
 * Combines data fetching and chart transformations
 */
export function usePerThread() {
  // Fetch data
  const { data, loading, error, refetch } = usePerThreadData()

  // Transform data for charts
  const { callCountChartData, timeChartData, memoryChartData, ticksPerSecond, threadCount } =
    usePerThreadCharts(computed(() => data.value))

  return {
    // Data
    data,
    loading,
    error,
    refetch,

    // Chart data
    callCountChartData,
    timeChartData,
    memoryChartData,

    // Metadata
    ticksPerSecond,
    threadCount,
  }
}

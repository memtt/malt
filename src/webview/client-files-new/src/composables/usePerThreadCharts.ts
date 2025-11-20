import { computed, type ComputedRef } from 'vue'
import type {
  PerThreadData,
  ThreadChartSeries,
  ThreadChartPoint,
  ChartMetric,
  AllocFunction,
} from '@/types/per-thread'

/**
 * Composable for transforming per-thread data into chart-ready format
 */
export function usePerThreadCharts(data: ComputedRef<PerThreadData | undefined>) {
  /**
   * Transform thread statistics for a specific metric
   * @param metric - Metric to extract ('count', 'time', or 'sum')
   * @returns Array of chart series (one per allocation function)
   */
  const transformThreadData = (metric: ChartMetric): ThreadChartSeries[] => {
    if (!data.value || !data.value.threadStats) return []

    const allocFunctions: readonly AllocFunction[] = [
      'malloc',
      'free',
      'calloc',
      'realloc',
      'posix_memalign',
      'aligned_alloc',
      'memalign',
      'valloc',
      'pvalloc',
    ]

    return allocFunctions.map((funcName) => {
      const values: ThreadChartPoint[] = data.value!.threadStats.map((threadStat, index) => ({
        name: `Thread ${index}`,
        value: threadStat[funcName][metric],
        threadId: index,
      }))

      return {
        key: funcName,
        values,
      }
    })
  }

  /**
   * Chart data for call count per thread
   */
  const callCountChartData = computed((): ThreadChartSeries[] => {
    return transformThreadData('count')
  })

  /**
   * Chart data for time per thread
   */
  const timeChartData = computed((): ThreadChartSeries[] => {
    return transformThreadData('time')
  })

  /**
   * Chart data for cumulated memory per thread
   */
  const memoryChartData = computed((): ThreadChartSeries[] => {
    return transformThreadData('sum')
  })

  /**
   * Ticks per second for time conversion
   */
  const ticksPerSecond = computed(() => data.value?.system.ticksPerSecond || 1)

  /**
   * Number of threads
   */
  const threadCount = computed(() => data.value?.threadStats.length || 0)

  return {
    callCountChartData,
    timeChartData,
    memoryChartData,
    ticksPerSecond,
    threadCount,
  }
}

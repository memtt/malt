/**
 * Data transformers for per-thread visualization
 */

import type {
  ThreadStats,
  ThreadChartSeries,
  ThreadChartPoint,
  ChartMetric,
  AllocFunction,
} from '@/types/per-thread'

/**
 * Transform thread statistics for a specific metric into chart format
 * @param threadStats - Array of thread statistics (one per thread)
 * @param metric - Metric to extract ('count', 'time', or 'sum')
 * @returns Array of chart series (one per allocation function)
 */
export function transformThreadStatsForChart(
  threadStats: ThreadStats[],
  metric: ChartMetric,
): ThreadChartSeries[] {
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
    const values: ThreadChartPoint[] = threadStats.map((threadStat, index) => ({
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

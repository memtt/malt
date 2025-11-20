/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/utils/per-thread/formatters.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
/**
 * Formatters for per-thread data
 */

import type { ChartMetric } from '@/types/per-thread'
import { humanReadable } from '@/lib/helpers'

/**
 * Format a metric value based on its type
 * @param value - Raw value to format
 * @param metric - Type of metric
 * @param ticksPerSecond - Conversion factor for time metric (default: 1)
 * @returns Formatted string
 */
export function formatMetricValue(
  value: number,
  metric: ChartMetric,
  ticksPerSecond: number = 1,
): string {
  switch (metric) {
    case 'count':
      // Format count without unit
      return humanReadable(value, 1, '', true)

    case 'time':
      // Convert ticks to seconds and format with 's' unit
      return humanReadable(value / ticksPerSecond, 1, 's', true)

    case 'sum':
      // Format memory size with 'B' unit
      return humanReadable(value, 1, 'B', true)

    default:
      return value.toString()
  }
}

/**
 * Get label for a metric type
 * @param metric - Type of metric
 * @returns Human-readable label
 */
export function getMetricLabel(metric: ChartMetric): string {
  switch (metric) {
    case 'count':
      return 'Call Count'
    case 'time':
      return 'Time (seconds)'
    case 'sum':
      return 'Memory (bytes)'
    default:
      return 'Value'
  }
}

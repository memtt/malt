/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/utils/timeline/formatters.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/
/**
 * Formatters for timeline data
 */

import type { ExportChartData, ChartDataPoint } from '@/types/timeline'
import { humanReadableTimes } from '@/lib/helpers'

/**
 * Format chart data to CSV string
 * Format: "Series1 timestamp" TAB "Series1" TAB "Series2 timestamp" TAB "Series2" ...
 * @param data - Chart data to export
 * @returns CSV string
 */
export function formatChartDataToCSV(data: ExportChartData): string {
  let buffer = ''

  // Header row
  for (let i = 0; i < data.labels.length; i++) {
    if (i > 0) buffer += '\t'
    buffer += `"${data.labels[i]} timestamp"\t"${data.labels[i]}"`
  }
  buffer += '\n'

  // Data rows
  const maxRows = Math.max(...data.data.map((series) => series.length))
  for (let row = 0; row < maxRows; row++) {
    for (let col = 0; col < data.data.length; col++) {
      if (col > 0) buffer += '\t'

      const point = data.data[col][row]
      if (point !== undefined) {
        buffer += `${point.x}\t${point.y}`
      } else {
        buffer += '\t'
      }
    }
    buffer += '\n'
  }

  return buffer
}

/**
 * Format chart data to JSON string
 * @param data - Chart data to export
 * @param pretty - Use pretty print (default: true)
 * @returns JSON string
 */
export function formatChartDataToJSON(data: ExportChartData, pretty: boolean = true): string {
  return JSON.stringify(data, null, pretty ? 2 : undefined)
}

/**
 * Format timestamp for display in tooltips
 * @param timestamp - Timestamp in ticks
 * @param ticksPerSecond - Conversion factor
 * @returns Formatted timestamp string
 */
export function formatTimestamp(timestamp: number, ticksPerSecond: number): string {
  return humanReadableTimes(timestamp, ticksPerSecond, 2, true)
}

/**
 * Format data point for display
 * @param point - Data point
 * @param ticksPerSecond - Conversion factor
 * @param valueFormatter - Custom formatter for Y value
 * @returns Formatted string
 */
export function formatDataPoint(
  point: ChartDataPoint,
  ticksPerSecond: number,
  valueFormatter?: (value: number) => string,
): string {
  const time = formatTimestamp(point.x, ticksPerSecond)
  const value = valueFormatter ? valueFormatter(point.y) : point.y.toFixed(2)
  return `Time: ${time}, Value: ${value}`
}

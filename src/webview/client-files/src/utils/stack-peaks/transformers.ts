/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/utils/stack-peaks/transformers.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
/**
 * Data transformers for stack peaks visualization
 */

import type {
  StackInfo,
  BarChartData,
  StackChartSeries,
  StackChartDataPoint,
  StackDetail,
} from '@/types/stack-peaks'

/**
 * Transform stack peaks data for horizontal bar chart
 * @param stacks - Array of stack info (one per thread)
 * @returns Array of bar chart data
 */
export function transformStackPeaksForChart(stacks: StackInfo[]): BarChartData[] {
  return stacks.map((stack, index) => ({
    label: `${index}`, // Thread ID as string
    value: stack.size,
    index,
  }))
}

/**
 * Transform stack timeline data for line chart
 * @param stack - Stack info for a specific thread
 * @param threadId - Thread ID for labeling
 * @returns Chart series with timeline data
 */
export function transformStackTimelineForChart(
  stack: StackInfo,
  threadId: number,
): StackChartSeries {
  const points: StackChartDataPoint[] = []

  for (let i = 0; i < stack.timeprofiler.timestamp.length; i++) {
    points.push({
      x: stack.timeprofiler.timestamp[i],
      y: stack.timeprofiler.max[i],
    })
  }

  return {
    key: `Thread ${threadId}`,
    values: points,
  }
}

/**
 * Transform all stacks timeline data for line chart (multi-series)
 * @param stacks - Array of stack info (all threads)
 * @returns Array of chart series
 */
export function transformAllStacksTimelineForChart(stacks: StackInfo[]): StackChartSeries[] {
  return stacks.map((stack, index) => transformStackTimelineForChart(stack, index))
}

/**
 * Transform stack details (function-level) data for horizontal bar chart
 * @param details - Array of stack details
 * @returns Array of bar chart data
 */
export function transformStackDetailsForChart(details: StackDetail[]): BarChartData[] {
  return details.map((detail, index) => ({
    label: detail.info.function,
    value: detail.mem,
    index,
    info: detail.info,
  }))
}

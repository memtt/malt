/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/composables/useStackPeaksSummary.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
import { computed, type ComputedRef } from 'vue'
import type {
  StackPeaksData,
  StackPeaksSummary,
  BarChartData,
  StackChartSeries,
  StackChartDataPoint,
  StackDetailsData,
} from '@/types/stack-peaks'

/**
 * Composable for computing summary statistics and chart data
 */
export function useStackPeaksSummary(
  data: ComputedRef<StackPeaksData | undefined>,
  selectedThreadId: ComputedRef<number | null>,
  detailsData: ComputedRef<StackDetailsData | undefined>,
) {
  /**
   * Summary statistics
   */
  const summary = computed((): StackPeaksSummary => {
    if (!data.value) {
      return { largestStack: 0, threadsCount: 0 }
    }

    // Find largest stack size
    let largestStack = 0
    for (const stack of data.value.stacks) {
      if (stack.size > largestStack) {
        largestStack = stack.size
      }
    }

    return {
      largestStack,
      threadsCount: data.value.stacks.length,
    }
  })

  /**
   * Chart data for stack peaks bar chart
   */
  const stackPeaksChartData = computed((): BarChartData[] => {
    if (!data.value) return []

    return data.value.stacks.map((stack, index) => ({
      label: `${index}`, // Thread ID
      value: stack.size,
      index,
    }))
  })

  /**
   * Chart data for stack timeline (line chart)
   */
  const stackTimelineChartData = computed((): StackChartSeries[] => {
    if (!data.value) return []

    const result: StackChartSeries[] = []

    // If a thread is selected, show only that thread
    // If no thread selected (null), show all threads
    if (selectedThreadId.value !== null) {
      const stack = data.value.stacks[selectedThreadId.value]
      if (stack) {
        const points: StackChartDataPoint[] = []
        for (let i = 0; i < stack.timeprofiler.timestamp.length; i++) {
          points.push({
            x: stack.timeprofiler.timestamp[i],
            y: stack.timeprofiler.max[i],
          })
        }
        result.push({
          key: `Thread ${selectedThreadId.value}`,
          values: points,
        })
      }
    } else {
      // Show all threads
      data.value.stacks.forEach((stack, index) => {
        const points: StackChartDataPoint[] = []
        for (let i = 0; i < stack.timeprofiler.timestamp.length; i++) {
          points.push({
            x: stack.timeprofiler.timestamp[i],
            y: stack.timeprofiler.max[i],
          })
        }
        result.push({
          key: `Thread ${index}`,
          values: points,
        })
      })
    }

    return result
  })

  /**
   * Chart data for stack details (function-level bar chart)
   */
  const stackDetailsChartData = computed((): BarChartData[] => {
    if (!detailsData.value) return []

    return detailsData.value.details.map((detail, index) => ({
      label: detail.info.function,
      value: detail.mem,
      index,
      info: detail.info,
    }))
  })

  /**
   * Ticks per second for time conversion
   */
  const ticksPerSecond = computed(() => data.value?.ticksPerSecond || 1)

  return {
    summary,
    stackPeaksChartData,
    stackTimelineChartData,
    stackDetailsChartData,
    ticksPerSecond,
  }
}

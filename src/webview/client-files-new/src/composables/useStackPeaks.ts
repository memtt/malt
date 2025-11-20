import { ref, computed } from 'vue'
import { useStackPeaksData } from '@/composables/useStackPeaksData'
import { useStackDetails } from '@/composables/useStackDetails'
import { useStackPeaksSummary } from '@/composables/useStackPeaksSummary'

/**
 * Main composable for Stack Peaks page
 * Combines data fetching, thread selection, and chart data preparation
 */
export function useStackPeaks() {
  // Selected thread ID (null = show all threads)
  const selectedThreadId = ref<number | null>(null)

  // Fetch main stack peaks data
  const { data, loading, error, refetch } = useStackPeaksData()

  // Fetch details for selected thread
  const {
    data: detailsData,
    loading: detailsLoading,
    error: detailsError,
  } = useStackDetails(selectedThreadId)

  // Compute summary and chart data
  const {
    summary,
    stackPeaksChartData,
    stackTimelineChartData,
    stackDetailsChartData,
    ticksPerSecond,
  } = useStackPeaksSummary(
    computed(() => data.value),
    computed(() => selectedThreadId.value),
    computed(() => detailsData.value),
  )

  /**
   * Select a thread by ID or null for all threads
   * @param threadId - Thread ID (0-based index) or null for all threads
   */
  const selectThread = (threadId: number | null): void => {
    selectedThreadId.value = threadId
  }

  /**
   * Deselect current thread (show all threads)
   */
  const deselectThread = (): void => {
    selectedThreadId.value = null
  }

  return {
    // Data
    data,
    loading,
    error,
    refetch,

    // Details
    detailsData,
    detailsLoading,
    detailsError,

    // Summary
    largestStack: computed(() => summary.value.largestStack),
    threadsCount: computed(() => summary.value.threadsCount),

    // Thread selection
    selectedThreadId: computed(() => selectedThreadId.value),
    selectThread,
    deselectThread,

    // Chart data
    stackPeaksChartData,
    stackTimelineChartData,
    stackDetailsChartData,
    ticksPerSecond,
  }
}

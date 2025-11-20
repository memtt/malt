import { computed, ref } from 'vue'
import { useTimelineData } from '@/composables/useTimelineData'
import { useTimelineCharts } from '@/composables/useTimelineCharts'
import { useTimelineExport } from '@/composables/useTimelineExport'
import { timelineService } from '@/services/timelineService'
import type { ChartSeries, ActiveChunksResponse } from '@/types/timeline'

/**
 * Main composable for Timeline page
 * Combines data fetching, chart transformations, and export functionality
 */
export function useTimeline() {
  // Data layer
  const { data, loading, error, refetch } = useTimelineData()

  // Charts layer
  const {
    memoryChartData,
    aliveChunksChartData,
    allocRateSizeChartData,
    allocRateCountChartData,
    systemMemoryChartData,
    toExportFormat,
  } = useTimelineCharts(computed(() => data.value))

  // Export layer
  const { downloadJSON, downloadCSV, exportToSVG } = useTimelineExport()

  // Modal state for allocated chunks
  const showChunksModal = ref(false)
  const chunksModalData = ref<ActiveChunksResponse | null>(null)
  const chunksModalLoading = ref(false)
  const chunksModalError = ref<Error | null>(null)

  /**
   * Get chart data by chart ID
   */
  const getChartData = (chartId: string): ChartSeries[] => {
    switch (chartId) {
      case 'memory':
        return memoryChartData.value
      case 'chunks':
        return aliveChunksChartData.value
      case 'alloc-rate-size':
        return allocRateSizeChartData.value
      case 'alloc-rate-count':
        return allocRateCountChartData.value
      case 'system':
        return systemMemoryChartData.value
      default:
        return []
    }
  }

  /**
   * Export chart data in specified format
   * @param chartId - ID of the chart to export
   * @param format - Export format: 'json', 'csv', or 'svg'
   * @param svgElement - Required for SVG export
   */
  const exportChart = (
    chartId: string,
    format: 'json' | 'csv' | 'svg',
    svgElement?: SVGSVGElement,
  ): void => {
    if (format === 'svg') {
      if (!svgElement) {
        console.error('SVG element is required for SVG export')
        return
      }
      exportToSVG(svgElement, chartId)
      return
    }

    // For JSON/CSV, download directly as files
    const chartData = getChartData(chartId)
    const exportData = toExportFormat(chartData)

    if (format === 'json') {
      downloadJSON(exportData, chartId)
    } else if (format === 'csv') {
      downloadCSV(exportData, chartId)
    }
  }

  /**
   * Fetch active chunks at a specific timestamp
   * @param timestamp - Timestamp in ticks
   */
  const fetchActiveChunks = async (timestamp: number): Promise<void> => {
    chunksModalLoading.value = true
    chunksModalError.value = null

    try {
      const result = await timelineService.getActiveChunks(Math.round(timestamp))
      chunksModalData.value = result
      showChunksModal.value = true
    } catch (err) {
      chunksModalError.value = err instanceof Error ? err : new Error('Failed to fetch chunks')
      console.error('Error fetching active chunks:', err)
    } finally {
      chunksModalLoading.value = false
    }
  }

  /**
   * Close chunks modal
   */
  const closeChunksModal = (): void => {
    showChunksModal.value = false
    chunksModalData.value = null
    chunksModalError.value = null
  }

  /**
   * Get ticks per second for timestamp conversion
   */
  const ticksPerSecond = computed(() => data.value?.ticksPerSecond || 1)

  return {
    // Data
    data,
    loading,
    error,
    refetch,
    ticksPerSecond,

    // Chart data
    memoryChartData,
    aliveChunksChartData,
    allocRateSizeChartData,
    allocRateCountChartData,
    systemMemoryChartData,

    // Export functionality
    exportChart,

    // Active chunks modal
    fetchActiveChunks,
    showChunksModal,
    chunksModalData,
    chunksModalLoading,
    chunksModalError,
    closeChunksModal,
  }
}

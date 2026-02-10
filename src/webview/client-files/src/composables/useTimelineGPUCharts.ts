/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/composables/useTimelineGPUCharts.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
import { computed, type ComputedRef } from 'vue'
import type {
  TimelineData,
  ChartSeries,
  ChartDataPoint,
  ExportChartData,
} from '@/types/timeline'

/**
 * Composable for transforming timeline data into chart-ready format
 */
export function useTimelineGPUCharts(data: ComputedRef<TimelineData | undefined>) {
  /**
   * Convert memory timeline data to chart format
   * Returns 3 series: Virtual memory, Physical memory, Requested memory
   */
  const memoryChartData = computed((): ChartSeries[] => {
    if (!data.value) return []

    const { memoryTimeline, ticksPerSecond: _ticksPerSecond } = data.value
    const colors = ['#7777ff', '#2ca02c', '#ff7f0e'] // Virtual, Physical, Requested

    // Create data points with initial (0,0) point
    const requestedGpuData: ChartDataPoint[] = [{ x: 0, y: 0 }]

    memoryTimeline.values.forEach((point, i) => {
      const timestamp = (i + 1) * memoryTimeline.perPoints
      requestedGpuData.push({ x: timestamp, y: point[5] || 0 })
    })

    return [
      { key: 'GPU requested memory', values: requestedGpuData, color: colors[0] },
    ]
  })

  /**
   * Convert alive chunks data to chart format
   * Returns 1 series: Memory segments count
   */
  const aliveChunksChartData = computed((): ChartSeries[] => {
    if (!data.value) return []

    const { memoryTimeline } = data.value
    const segmentsData: ChartDataPoint[] = [{ x: 0, y: 0 }]

    memoryTimeline.values.forEach((point, i) => {
      const timestamp = (i + 1) * memoryTimeline.perPoints
      segmentsData.push({ x: timestamp, y: point[6] || 0 }) // Index 4 = count
    })

    return [{ key: 'GPU memory segments', values: segmentsData, color: '#ff7f0e' }]
  })

  /**
   * Convert allocation rate (size) data to chart format
   * Returns 2 series: Malloc bandwidth, Free bandwidth (in B/s)
   */
  const allocRateSizeChartData = computed((): ChartSeries[] => {
    if (!data.value) return []

    const { memoryGpuBandwidth, ticksPerSecond } = data.value
    const scale = memoryGpuBandwidth.perPoints / 10 // Note from Angular: why /10?
    const colors = ['#ff7f0e', '#2ca02c']

    const mallocData: ChartDataPoint[] = [{ x: 0, y: 0 }]
    const freeData: ChartDataPoint[] = [{ x: 0, y: 0 }]

    memoryGpuBandwidth.values.forEach((point, i) => {
      const timestamp = scale * (i + 1)
      const allocSize = point[1] || 0 // Index 1 = allocSize
      const freeSize = point[3] || 0 // Index 3 = freeSize

      // Calculate rate: value / (perPoints / ticksPerSecond)
      const allocRate = allocSize / (memoryGpuBandwidth.perPoints / ticksPerSecond)
      const freeRate = freeSize / (memoryGpuBandwidth.perPoints / ticksPerSecond)

      mallocData.push({ x: timestamp, y: allocRate })
      freeData.push({ x: timestamp, y: freeRate })
    })

    return [
      { key: 'Malloc', values: mallocData, color: colors[0] },
      { key: 'Free', values: freeData, color: colors[1] },
    ]
  })

  /**
   * Convert allocation rate (count) data to chart format
   * Returns 2 series: Malloc operations, Free operations (in op/s)
   */
  const allocRateCountChartData = computed((): ChartSeries[] => {
    if (!data.value) return []

    const { memoryGpuBandwidth, ticksPerSecond } = data.value
    const scale = memoryGpuBandwidth.perPoints / 10 // Note from Angular: why /10?
    const colors = ['#ff7f0e', '#2ca02c']

    const mallocData: ChartDataPoint[] = [{ x: 0, y: 0 }]
    const freeData: ChartDataPoint[] = [{ x: 0, y: 0 }]

    memoryGpuBandwidth.values.forEach((point, i) => {
      const timestamp = scale * (i + 1)
      const allocCount = point[0] || 0 // Index 0 = allocCount
      const freeCount = point[2] || 0 // Index 2 = freeCount

      // Calculate rate: value / (perPoints / ticksPerSecond)
      const allocRate = allocCount / (memoryGpuBandwidth.perPoints / ticksPerSecond)
      const freeRate = freeCount / (memoryGpuBandwidth.perPoints / ticksPerSecond)

      mallocData.push({ x: timestamp, y: allocRate })
      freeData.push({ x: timestamp, y: freeRate })
    })

    return [
      { key: 'Malloc', values: mallocData, color: colors[0] },
      { key: 'Free', values: freeData, color: colors[1] },
    ]
  })

  /**
   * Convert system memory data to chart format
   * Returns 2 series: System free memory, Swap memory
   */
  const systemMemoryChartData = computed((): ChartSeries[] => {
    if (!data.value) return []

    const { systemTimeline } = data.value
    const colors = ['#ff7f0e', '#2ca02c']

    const freeMemData: ChartDataPoint[] = [{ x: 0, y: 0 }]
    const swapData: ChartDataPoint[] = [{ x: 0, y: 0 }]

    systemTimeline.values.forEach((point, i) => {
      const timestamp = (i + 1) * systemTimeline.perPoints
      freeMemData.push({ x: timestamp, y: point[0] || 0 }) // Index 0 = free
      swapData.push({ x: timestamp, y: point[1] || 0 }) // Index 1 = swap
    })

    return [
      { key: 'System free memory', values: freeMemData, color: colors[0] },
      { key: 'Swap memory', values: swapData, color: colors[1] },
    ]
  })

  /**
   * Convert chart series to export format
   */
  const toExportFormat = (series: ChartSeries[]): ExportChartData => {
    return {
      labels: series.map((s) => s.key),
      ticksPerSecond: data.value?.ticksPerSecond || 1,
      data: series.map((s) => s.values),
    }
  }

  return {
    memoryChartData,
    aliveChunksChartData,
    allocRateSizeChartData,
    allocRateCountChartData,
    systemMemoryChartData,
    toExportFormat,
  }
}

/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/composables/useAllocSizeDistrHeatmaps.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
import { computed, type ComputedRef } from 'vue'
import type { ScatterData } from '@/types/alloc-size-distr'
import type { HeatmapCell } from '@/components/charts/HeatmapChart.vue'

/**
 * Transform scatter data into heatmap bins
 * Maps each scatter point to a bin based on its X/Y position
 * Color intensity is based on the point's 'v' value (circle size in scatter plot)
 */
export function useAllocSizeDistrHeatmaps(
  scatterData: ComputedRef<ScatterData | undefined>,
  binsX = 50,
  binsY = 50,
) {
  const sizeOverTimeHeatmap = computed(() => {
    if (!scatterData.value?.sizeOverTime?.points) return []

    const points = scatterData.value.sizeOverTime.points
    if (points.length === 0) return []

    // Find min/max for X (time) and Y (size)
    const xValues = points.map((p) => p.x)
    const yValues = points.map((p) => p.y)

    const xMin = Math.min(...xValues)
    const xMax = Math.max(...xValues)
    const yMin = Math.min(...yValues)
    const yMax = Math.max(...yValues)

    // Use log scale for Y (size in bytes)
    const yMinLog = Math.log10(Math.max(yMin, 1))
    const yMaxLog = Math.log10(Math.max(yMax, 1))

    // Create bins
    const xBinSize = (xMax - xMin) / binsX
    const yBinSizeLog = (yMaxLog - yMinLog) / binsY

    // Create ALL bins (full grid)
    const heatmapCells: HeatmapCell[] = []
    for (let xBin = 0; xBin < binsX; xBin++) {
      for (let yBin = 0; yBin < binsY; yBin++) {
        const xBinMin = xMin + xBin * xBinSize
        const xBinMax = xMin + (xBin + 1) * xBinSize
        const yBinMinLog = yMinLog + yBin * yBinSizeLog
        const yBinMaxLog = yMinLog + (yBin + 1) * yBinSizeLog

        heatmapCells.push({
          x: xBin,
          y: yBin,
          value: 0,
          xMin: xBinMin,
          xMax: xBinMax,
          yMin: Math.pow(10, yBinMinLog),
          yMax: Math.pow(10, yBinMaxLog),
        })
      }
    }

    // Now fill in the values from scatter points
    points.forEach((point) => {
      const xBin = Math.floor((point.x - xMin) / xBinSize)
      const yLog = Math.log10(Math.max(point.y, 1))
      const yBin = Math.floor((yLog - yMinLog) / yBinSizeLog)

      const xBinClamped = Math.max(0, Math.min(binsX - 1, xBin))
      const yBinClamped = Math.max(0, Math.min(binsY - 1, yBin))

      // Find the cell index in the flat array
      const cellIndex = xBinClamped * binsY + yBinClamped
      if (cellIndex >= 0 && cellIndex < heatmapCells.length) {
        heatmapCells[cellIndex].value += point.v
      }
    })

    return heatmapCells
  })

  const lifetimeOverSizeHeatmap = computed(() => {
    if (!scatterData.value?.lifetimeOverSize?.points) return []

    const points = scatterData.value.lifetimeOverSize.points
    if (points.length === 0) return []

    // Find min/max for X (size) and Y (lifetime)
    const xValues = points.map((p) => p.x)
    const yValues = points.map((p) => p.y)

    const xMin = Math.min(...xValues)
    const xMax = Math.max(...xValues)
    const yMin = Math.min(...yValues)
    const yMax = Math.max(...yValues)

    // Use log scale for both X (size) and Y (lifetime)
    const xMinLog = Math.log10(Math.max(xMin, 1))
    const xMaxLog = Math.log10(Math.max(xMax, 1))
    const yMinLog = Math.log10(Math.max(yMin, 1))
    const yMaxLog = Math.log10(Math.max(yMax, 1))

    // Create bins
    const xBinSizeLog = (xMaxLog - xMinLog) / binsX
    const yBinSizeLog = (yMaxLog - yMinLog) / binsY

    // Create ALL bins (full grid)
    const heatmapCells: HeatmapCell[] = []
    for (let xBin = 0; xBin < binsX; xBin++) {
      for (let yBin = 0; yBin < binsY; yBin++) {
        const xBinMinLog = xMinLog + xBin * xBinSizeLog
        const xBinMaxLog = xMinLog + (xBin + 1) * xBinSizeLog
        const yBinMinLog = yMinLog + yBin * yBinSizeLog
        const yBinMaxLog = yMinLog + (yBin + 1) * yBinSizeLog

        heatmapCells.push({
          x: xBin,
          y: yBin,
          value: 0,
          xMin: Math.pow(10, xBinMinLog),
          xMax: Math.pow(10, xBinMaxLog),
          yMin: Math.pow(10, yBinMinLog),
          yMax: Math.pow(10, yBinMaxLog),
        })
      }
    }

    // Now fill in the values from scatter points
    points.forEach((point) => {
      const xLog = Math.log10(Math.max(point.x, 1))
      const yLog = Math.log10(Math.max(point.y, 1))
      const xBin = Math.floor((xLog - xMinLog) / xBinSizeLog)
      const yBin = Math.floor((yLog - yMinLog) / yBinSizeLog)

      const xBinClamped = Math.max(0, Math.min(binsX - 1, xBin))
      const yBinClamped = Math.max(0, Math.min(binsY - 1, yBin))

      // Find the cell index in the flat array
      const cellIndex = xBinClamped * binsY + yBinClamped
      if (cellIndex >= 0 && cellIndex < heatmapCells.length) {
        heatmapCells[cellIndex].value += point.v
      }
    })

    return heatmapCells
  })

  return {
    sizeOverTimeHeatmap,
    lifetimeOverSizeHeatmap,
  }
}

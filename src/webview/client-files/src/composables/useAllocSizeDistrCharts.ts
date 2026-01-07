/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/composables/useAllocSizeDistrCharts.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
import { computed, type Ref } from 'vue'
import type { SizeMapData, MostUsedSizeItem } from '@/types/alloc-size-distr'
import { buildLog2Histogram, type Log2HistogramData } from '@/utils/alloc-size-distr/transformers'

export function useAllocSizeDistrCharts(data: Ref<SizeMapData | undefined>) {
  const mostUsedSizesData = computed<MostUsedSizeItem[]>(() => {
    if (!data.value) return []

    const formattedData: MostUsedSizeItem[] = []
    let max = 0

    // Convert to array and find max
    const tmp: MostUsedSizeItem[] = []
    for (const sizeStr in data.value) {
      const size = Number(sizeStr)
      const frequency = data.value[sizeStr]
      tmp.push({ size, frequency })
      if (frequency > max) {
        max = frequency
      }
    }

    // Sort by frequency descending
    tmp.sort((a, b) => b.frequency - a.frequency)

    // Keep top items (frequency > max/1000 and cnt < 50)
    let cnt = 0
    let others = 0
    for (const item of tmp) {
      if (item.frequency > max / 1000 && cnt < 50) {
        formattedData.push(item)
        cnt++
      } else {
        others += item.frequency
      }
    }

    // Add "others" category
    if (others > 0) {
      formattedData.push({ size: 0, frequency: others })
    }

    // Sort again by frequency descending
    formattedData.sort((a, b) => b.frequency - a.frequency)

    return formattedData
  })

  const histogramData = computed<Log2HistogramData[]>(() => {
    if (!data.value) return []
    return buildLog2Histogram(data.value)
  })

  return {
    mostUsedSizesData,
    histogramData,
  }
}

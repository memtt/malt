/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/composables/useAllocSizeDistrSummary.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
import { computed, type Ref } from 'vue'
import type { SizeMapData } from '@/types/alloc-size-distr'
import { humanReadable } from '@/lib/helpers'

export function useAllocSizeDistrSummary(data: Ref<SizeMapData | undefined>) {
  const meanChunkSize = computed(() => {
    if (!data.value) return '0 B'

    let sumSize = 0
    let sumCount = 0

    for (const sizeStr in data.value) {
      const size = Number(sizeStr)
      const count = data.value[sizeStr]
      sumSize += size * count
      sumCount += count
    }

    if (sumCount === 0) return '0 B'
    return humanReadable(sumSize / sumCount, 1, 'B', true)
  })

  const mostUsedSize = computed(() => {
    if (!data.value) return '0 B'

    let mostUsed = -1
    let maxCount = 0

    for (const sizeStr in data.value) {
      const size = Number(sizeStr)
      const count = data.value[sizeStr]
      if (count > maxCount) {
        maxCount = count
        mostUsed = size
      }
    }

    return mostUsed === -1 ? '0 B' : humanReadable(mostUsed, 1, 'B', true)
  })

  const largestSize = computed(() => {
    if (!data.value) return '0 B'

    let largest = 0

    for (const sizeStr in data.value) {
      const size = Number(sizeStr)
      if (size > largest) {
        largest = size
      }
    }

    return humanReadable(largest, 1, 'B', true)
  })

  return {
    meanChunkSize,
    mostUsedSize,
    largestSize,
  }
}

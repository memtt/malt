/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/utils/alloc-size-distr/transformers.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
import type { SizeMapData } from '@/types/alloc-size-distr'

export interface Log2HistogramData {
  sizeLog: number
  count: number
}

/**
 * Build log2 histogram from size map data
 * Groups sizes by their log2 value (matching Angular implementation)
 */
export function buildLog2Histogram(data: SizeMapData): Log2HistogramData[] {
  const tmp: { [key: number]: number } = {}

  for (const sizeStr in data) {
    const size = Number(sizeStr)
    const frequency = data[sizeStr]

    // Handle special case: size = 0
    // For size > 0, calculate log2 bucket
    const id = size === 0 ? 0 : Math.ceil(Math.log(size) / Math.log(2))

    if (tmp[id] === undefined) {
      tmp[id] = frequency
    } else {
      tmp[id] += frequency
    }
  }

  // Convert to array format
  const hist: Log2HistogramData[] = []
  for (const i in tmp) {
    hist.push({
      sizeLog: Number(i),
      count: tmp[i],
    })
  }

  // Sort by sizeLog for proper display
  hist.sort((a, b) => a.sizeLog - b.sizeLog)

  return hist
}

/**
 * Composable for Realloc Charts
 * Responsible ONLY for formatting chart data
 */

import { computed, type Ref } from 'vue'
import type { ReallocData } from '@/types/realloc'
import type { BarSeries } from '@/types/global-vars'
import {
  generateDeltaDistribution,
  buildLog2Histogram,
  getTopMostUsed,
  getTopDeltas,
} from '@/utils/realloc/calculators'

export function useReallocCharts(data: Ref<ReallocData>) {
  // Delta distribution (used by multiple charts)
  const deltaDistribution = computed(() => generateDeltaDistribution(data.value))

  // Log2 histogram data
  const log2Histogram = computed(() => buildLog2Histogram(deltaDistribution.value))

  // Top 20 most used realloc sizes
  const topMostUsed = computed(() => getTopMostUsed(data.value, 20))

  // Top deltas for histogram
  const topDeltas = computed(() => getTopDeltas(deltaDistribution.value, 50))

  // Format data for bar chart
  const mostUsedBarData = computed((): BarSeries[] => {
    if (topMostUsed.value.length === 0) return []

    return [
      {
        key: 'Realloc',
        color: '#4682b4',
        values: topMostUsed.value.map((entry) => ({
          name: entry.label,
          value: entry.count,
        })),
      },
    ]
  })

  return {
    deltaDistribution,
    log2Histogram,
    topMostUsed,
    topDeltas,
    mostUsedBarData,
  }
}

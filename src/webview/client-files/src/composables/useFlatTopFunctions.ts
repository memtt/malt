/**
 * Composable to compute top functions by different metrics
 */

import { computed, type ComputedRef } from 'vue'
import type { FlatFunction, MetricType } from '@/types/flat'
import { getMetricValue, getFormattedMetricValue, computeMetricRef, METRICS } from '@/utils/flat/metrics'
import type { RankingItem } from '@/components/home/FunctionRankingList.vue'

interface TopFunctionsOptions {
  limit?: number
  inclusive?: boolean
}

/**
 * Get top functions for a specific metric
 */
export function useFlatTopFunctions(
  data: ComputedRef<FlatFunction[] | undefined>,
  metric: MetricType,
  options: TopFunctionsOptions = {}
) {
  const { limit = 5, inclusive = false } = options

  const topFunctions = computed<RankingItem[]>(() => {
    if (!data.value || data.value.length === 0) {
      return []
    }

    const metricConfig = METRICS[metric]
    const functions = data.value

    // Map to ranking items with values
    const items = functions
      .map((fn) => {
        // Extract stats based on inclusive flag
        // If inclusive=false and own doesn't exist, return null (will be filtered out)
        let fnStats
        if (inclusive) {
          fnStats = fn.total
        } else {
          if (!fn.own) {
            return null
          }
          fnStats = fn.own
        }

        const value = getMetricValue(fnStats, metric)
        return {
          fn,
          fnStats,
          value,
        }
      })
      .filter((item): item is NonNullable<typeof item> => item !== null && item.value > 0) // Hide zero values and null items

    // Compute reference value for percentage calculation (only from valid items)
    const validStats = items.map(item => item.fnStats)
    const refValue = computeMetricRef(validStats, metric)

    // Sort based on metric order
    items.sort((a, b) => {
      if (metricConfig.defaultOrder === 'desc') {
        return b.value - a.value
      } else {
        return a.value - b.value
      }
    })

    // Limit and format
    return items.slice(0, limit).map((item) => ({
      function: item.fn.function,
      value: getFormattedMetricValue(item.fnStats, metric),
      ratio: refValue > 0 ? (item.value / refValue) * 100 : 0,
      link: `/sources?func=${encodeURIComponent(item.fn.function)}&metric=${metric}`,
    }))
  })

  return {
    topFunctions,
  }
}

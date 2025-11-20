/**
 * Composable for Sources - Composition Layer
 * Combines data and metric selector composables
 */

import { useSourcesData } from '@/composables/useSourcesData'
import { useMetricSelector } from '@/composables/useMetricSelector'
import type { MetricKey } from '@/types/sources'

export interface UseSourcesOptions {
  initialMetric?: MetricKey
  initialInclusive?: boolean
  initialPerPage?: number
}

export function useSources(options: UseSourcesOptions = {}) {
  // Get data
  const { data, loading, error, refetch } = useSourcesData()

  // Get selector
  const selector = useMetricSelector(data, {
    initialMetric: options.initialMetric,
    initialInclusive: options.initialInclusive,
    initialPerPage: options.initialPerPage,
  })

  return {
    // Data state
    loading,
    error,
    refetch,

    // Functions
    functions: data,
    filteredFunctions: selector.filteredFunctions,

    // Selector state and methods
    selector,
  }
}

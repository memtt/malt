/**
 * Composable for Realloc Data Fetching
 * Responsible ONLY for fetching data from API
 */

import { useQuery } from '@tanstack/vue-query'
import { computed } from 'vue'
import { fetchRealloc } from '@/services/reallocService'

export function useReallocData() {
  const query = useQuery({
    queryKey: ['realloc'],
    queryFn: fetchRealloc,
  })

  return {
    data: computed(() => query.data.value || []),
    loading: computed(() => query.isLoading.value),
    error: computed(() => query.error.value?.message || null),
    refetch: query.refetch,
  }
}

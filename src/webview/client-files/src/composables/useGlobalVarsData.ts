/**
 * Composable for Global Variables Data Fetching
 * Responsible ONLY for fetching data from API
 */

import { useQuery } from '@tanstack/vue-query'
import { computed } from 'vue'
import { fetchGlobalVars } from '@/services/globalVarsService'

export function useGlobalVarsData() {
  const query = useQuery({
    queryKey: ['globalVars'],
    queryFn: fetchGlobalVars,
  })

  return {
    data: computed(() => query.data.value),
    loading: computed(() => query.isLoading.value),
    error: computed(() => query.error.value?.message || null),
    refetch: query.refetch,
  }
}

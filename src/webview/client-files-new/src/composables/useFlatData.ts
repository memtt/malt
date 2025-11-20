/**
 * Data Layer: Fetch flat function statistics
 */

import { useQuery } from '@tanstack/vue-query'
import { fetchFlatFunctions } from '@/services/flatService'

export function useFlatData() {
  const { data, isLoading, error, refetch } = useQuery({
    queryKey: ['flat'],
    queryFn: fetchFlatFunctions,
    staleTime: 1000 * 60 * 5, // 5 minutes
  })

  return {
    data,
    loading: isLoading,
    error,
    refetch,
  }
}

import { useQuery } from '@tanstack/vue-query'
import { fetchSizeDistr } from '@/services/allocSizeDistrService'

export function useAllocSizeDistrData() {
  const { data, isLoading, error } = useQuery({
    queryKey: ['allocSizeDistr'],
    queryFn: fetchSizeDistr,
  })

  return {
    data,
    loading: isLoading,
    error,
  }
}

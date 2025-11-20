import { useQuery } from '@tanstack/vue-query'
import { fetchScatterData } from '@/services/allocSizeDistrService'

export function useAllocSizeDistrScatterData() {
  const { data, isLoading, error } = useQuery({
    queryKey: ['allocSizeDistrScatter'],
    queryFn: fetchScatterData,
  })

  return {
    data,
    loading: isLoading,
    error,
  }
}

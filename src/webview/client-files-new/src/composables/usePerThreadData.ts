import { useQuery } from '@tanstack/vue-query'
import { perThreadService } from '@/services/perThreadService'
import type { PerThreadData } from '@/types/per-thread'

/**
 * Composable for fetching per-thread data
 * Uses TanStack Query for caching and loading states
 */
export function usePerThreadData() {
  const {
    data,
    isLoading: loading,
    error,
    refetch,
  } = useQuery<PerThreadData>({
    queryKey: ['perThread'],
    queryFn: () => perThreadService.getPerThreadData(),
    staleTime: 5 * 60 * 1000, // 5 minutes
  })

  return {
    data,
    loading,
    error,
    refetch,
  }
}

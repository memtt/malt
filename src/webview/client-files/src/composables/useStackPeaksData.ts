import { useQuery } from '@tanstack/vue-query'
import { stackPeaksService } from '@/services/stackPeaksService'
import type { StackPeaksData } from '@/types/stack-peaks'

/**
 * Composable for fetching stack peaks data
 * Uses TanStack Query for caching and loading states
 */
export function useStackPeaksData() {
  const {
    data,
    isLoading: loading,
    error,
    refetch,
  } = useQuery<StackPeaksData>({
    queryKey: ['stackPeaks'],
    queryFn: () => stackPeaksService.getStackPeaksData(),
    staleTime: 5 * 60 * 1000, // 5 minutes
  })

  return {
    data,
    loading,
    error,
    refetch,
  }
}

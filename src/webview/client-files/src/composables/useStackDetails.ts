import { computed, type Ref } from 'vue'
import { useQuery } from '@tanstack/vue-query'
import { stackPeaksService } from '@/services/stackPeaksService'
import type { StackDetailsData } from '@/types/stack-peaks'

/**
 * Composable for fetching stack details for a specific thread
 * Query is only enabled when a thread is selected
 */
export function useStackDetails(selectedThreadId: Ref<number | null>) {
  const {
    data,
    isLoading: loading,
    error,
    refetch,
  } = useQuery<StackDetailsData>({
    queryKey: computed(() => ['stackDetails', selectedThreadId.value]),
    queryFn: () => {
      if (selectedThreadId.value === null) {
        throw new Error('No thread selected')
      }
      return stackPeaksService.getStackDetails(selectedThreadId.value)
    },
    enabled: computed(() => selectedThreadId.value !== null),
    staleTime: 5 * 60 * 1000, // 5 minutes
  })

  return {
    data,
    loading,
    error,
    refetch,
  }
}

import { useQuery } from '@tanstack/vue-query'
import { timelineService } from '@/services/timelineService'
import type { TimelineData } from '@/types/timeline'

/**
 * Composable for fetching timeline data
 * Uses TanStack Query for caching and loading states
 */
export function useTimelineData() {
  const {
    data,
    isLoading: loading,
    error,
    refetch,
  } = useQuery<TimelineData>({
    queryKey: ['timeline'],
    queryFn: () => timelineService.getTimelineData(),
    staleTime: 5 * 60 * 1000, // 5 minutes
  })

  return {
    data,
    loading,
    error,
    refetch,
  }
}

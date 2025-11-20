/**
 * Composable for Home Data Fetching
 * Responsible ONLY for fetching home summary data from API
 */

import { useQuery } from '@tanstack/vue-query';
import { computed } from 'vue';
import { fetchHomeSummary } from '@/services/homeService';

export function useHomeData() {
  const query = useQuery({
    queryKey: ['homeSummary'],
    queryFn: fetchHomeSummary,
  });

  return {
    data: computed(() => query.data.value),
    loading: computed(() => query.isLoading.value),
    error: computed(() => query.error.value?.message || null),
    refetch: query.refetch,
  };
}

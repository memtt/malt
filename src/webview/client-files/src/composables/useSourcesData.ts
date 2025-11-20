/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/composables/useSourcesData.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
/**
 * Composable for Sources Data Fetching
 * Responsible ONLY for fetching function list from API
 */

import { useQuery } from '@tanstack/vue-query';
import { computed } from 'vue';
import { fetchFlatFunctions } from '@/services/sourcesService';

export function useSourcesData() {
  const query = useQuery({
    queryKey: ['flatFunctions'],
    queryFn: fetchFlatFunctions,
  });

  return {
    data: computed(() => query.data.value || []),
    loading: computed(() => query.isLoading.value),
    error: computed(() => query.error.value?.message || null),
    refetch: query.refetch,
  };
}

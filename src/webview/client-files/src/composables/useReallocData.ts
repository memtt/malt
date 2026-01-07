/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/composables/useReallocData.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
/**
 * Composable for Realloc Data Fetching
 * Responsible ONLY for fetching data from API
 */

import { useQuery } from '@tanstack/vue-query'
import { computed } from 'vue'
import { fetchRealloc } from '@/services/reallocService'

export function useReallocData() {
  const query = useQuery({
    queryKey: ['realloc'],
    queryFn: fetchRealloc,
  })

  return {
    data: computed(() => query.data.value || []),
    loading: computed(() => query.isLoading.value),
    error: computed(() => query.error.value?.message || null),
    refetch: query.refetch,
  }
}

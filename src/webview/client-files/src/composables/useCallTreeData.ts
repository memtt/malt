/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/composables/useCallTreeData.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
/**
 * Composable for Call Tree Data Fetching
 * Responsible ONLY for fetching call tree data from API
 */

import { useQuery } from '@tanstack/vue-query';
import { computed, type Ref } from 'vue';
import { fetchCallTree } from '@/services/callTreeService';
import type { CallTreeParams } from '@/types/call-tree';

interface UseCallTreeDataOptions {
  params: Ref<CallTreeParams>;
  enabled?: Ref<boolean>;
}

export function useCallTreeData(options: UseCallTreeDataOptions) {
  const query = useQuery({
    queryKey: computed(() => ['callTree', options.params.value]),
    queryFn: () => fetchCallTree(options.params.value),
    enabled: options.enabled,
  });

  return {
    data: computed(() => query.data.value),
    loading: computed(() => query.isLoading.value),
    error: computed(() => query.error.value?.message || null),
    refetch: query.refetch,
  };
}

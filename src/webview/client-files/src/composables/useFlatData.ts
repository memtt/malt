/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/composables/useFlatData.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
/**
 * Data Layer: Fetch flat function statistics
 */

import { useQuery } from '@tanstack/vue-query'
import { fetchFlatFunctions } from '@/services/flatService'

export function useFlatData() {
  const { data, isLoading, error, refetch } = useQuery({
    queryKey: ['flat'],
    queryFn: fetchFlatFunctions,
    staleTime: 1000 * 60 * 5, // 5 minutes
  })

  return {
    data,
    loading: isLoading,
    error,
    refetch,
  }
}

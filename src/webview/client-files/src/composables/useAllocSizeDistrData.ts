/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/composables/useAllocSizeDistrData.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
import { useQuery } from '@tanstack/vue-query'
import { fetchSizeDistr } from '@/services/allocSizeDistrService'

export function useAllocSizeDistrData() {
  const { data, isLoading, error } = useQuery({
    queryKey: ['allocSizeDistr'],
    queryFn: fetchSizeDistr,
  })

  return {
    data,
    loading: isLoading,
    error,
  }
}

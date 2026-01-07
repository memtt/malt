/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/composables/useAllocSizeDistrScatterData.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
import { useQuery } from '@tanstack/vue-query'
import { fetchScatterData } from '@/services/allocSizeDistrService'

export function useAllocSizeDistrScatterData() {
  const { data, isLoading, error } = useQuery({
    queryKey: ['allocSizeDistrScatter'],
    queryFn: fetchScatterData,
  })

  return {
    data,
    loading: isLoading,
    error,
  }
}

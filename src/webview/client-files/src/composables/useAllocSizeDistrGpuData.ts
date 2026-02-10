/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/composables/useAllocSizeDistrData.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
import { useQuery } from '@tanstack/vue-query'
import { fetchSizeDistrGpu } from '@/services/allocSizeDistrGpuService'

export function useAllocSizeDistrGpuData() {
  const { data, isLoading, error } = useQuery({
    queryKey: ['allocSizeDistr'],
    queryFn: fetchSizeDistrGpu,
  })

  return {
    data,
    loading: isLoading,
    error,
  }
}

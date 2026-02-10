/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/composables/useAllocSizeDistrScatterData.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
import { useQuery } from '@tanstack/vue-query'
import { fetchScatterDataGpu } from '@/services/allocSizeDistrGpuService'

export function useAllocSizeDistrGpuScatterData() {
  const { data, isLoading, error } = useQuery({
    queryKey: ['allocSizeDistrGpuScatter'],
    queryFn: fetchScatterDataGpu,
  })

  return {
    data,
    loading: isLoading,
    error,
  }
}

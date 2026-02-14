/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/composables/useAllocSizeDistr.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
import { useAllocSizeDistrGpuData } from '@/composables/useAllocSizeDistrGpuData'
import { useAllocSizeDistrGpuScatterData } from '@/composables/useAllocSizeDistrGpuScatterData'
import { useAllocSizeDistrSummary } from '@/composables/useAllocSizeDistrSummary'
import { useAllocSizeDistrCharts } from '@/composables/useAllocSizeDistrCharts'
import { computed, type Ref } from 'vue'
import type { SizeMapData } from '@/types/alloc-size-distr'

function useAllocSizeDistrHaveGpu(data: Ref<SizeMapData | undefined>) {
  const haveGpu = computed(() => {
    if (!data)
        return false;
    let sumCount = 0
    for (const sizeStr in data.value) {
        const count = data.value[sizeStr]
        sumCount += count
    }
    if (sumCount > 0)
        return true;
    else
        return false;
  });
  return haveGpu;
}

export function useAllocSizeDistrGpu() {
  const { data, loading, error } = useAllocSizeDistrGpuData()
  const {
    data: scatterData,
    loading: scatterLoading,
    error: scatterError,
  } = useAllocSizeDistrGpuScatterData();
  const { meanChunkSize, mostUsedSize, largestSize } = useAllocSizeDistrSummary(data)
  const { mostUsedSizesData, histogramData } = useAllocSizeDistrCharts(data)
  const haveGpu = useAllocSizeDistrHaveGpu(data);

  return {
    data,
    loading,
    error,
    scatterData,
    scatterLoading,
    scatterError,
    meanChunkSize,
    mostUsedSize,
    largestSize,
    mostUsedSizesData,
    histogramData,
    haveGpu,
  }
}

import { useAllocSizeDistrData } from '@/composables/useAllocSizeDistrData'
import { useAllocSizeDistrScatterData } from '@/composables/useAllocSizeDistrScatterData'
import { useAllocSizeDistrSummary } from '@/composables/useAllocSizeDistrSummary'
import { useAllocSizeDistrCharts } from '@/composables/useAllocSizeDistrCharts'

export function useAllocSizeDistr() {
  const { data, loading, error } = useAllocSizeDistrData()
  const {
    data: scatterData,
    loading: scatterLoading,
    error: scatterError,
  } = useAllocSizeDistrScatterData()
  const { meanChunkSize, mostUsedSize, largestSize } = useAllocSizeDistrSummary(data)
  const { mostUsedSizesData, histogramData } = useAllocSizeDistrCharts(data)

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
  }
}

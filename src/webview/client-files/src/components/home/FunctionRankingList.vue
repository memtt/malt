<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/home/FunctionRankingList.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
|    AUTHOR   : Sébastien Valat (INRIA) - 2025
----------------------------------------------------------->
<template>
  <FunctionList
    :functions="convertedFunctions"
    :all-functions="convertedFunctions"
    :get-formatted-value="getFormattedValue"
    :get-value="getValue"
    :show-header="true"
    :show-pagination="false"
    :compact="true"
    :use-link-navigation="true"
    :get-link-for-function="getLinkForFunction"
    empty-message="No data available"
  />
</template>

<script setup lang="ts">
import { computed } from 'vue'
import FunctionList from '@/components/shared/FunctionList.vue'
import type { FunctionStat } from '@/types/sources'

export interface RankingItem {
  value: string
  ratio: number
  function: string
  link: string
}

interface Props {
  items: RankingItem[]
}

const props = defineProps<Props>()

// Convert RankingItem[] to FunctionStat[] for FunctionList
const convertedFunctions = computed<FunctionStat[]>(() => {
  return props.items.map((item) => ({
    function: item.function,
    functionShort: item.function,
    file: '',
    line: 0,
    total: {
      countZeros: 0,
      maxAliveReq: 0,
      aliveReq: 0,
	  maxAliveReqGPU: 0,
      aliveReqGPU: 0,
      alloc: {
        sum: parseValueToNumber(item.value),
        count: 0,
        min: 0,
        max: 0,
      },
      free: {
        sum: 0,
        count: 0,
        min: 0,
        max: 0,
      },
	  gpuAlloc: {
        sum: 0,
        count: 0,
        min: 0,
        max: 0,
      },
      gpuFree: {
        sum: 0,
        count: 0,
        min: 0,
        max: 0,
      },
      lifetime: {
        count: 0,
        sum: 0,
        min: 0,
        max: 0,
      },
      mmap: {
        count: 0,
        sum: 0,
        min: 0,
        max: 0,
      },
      munmap: {
        count: 0,
        sum: 0,
        min: 0,
        max: 0,
      },
    },
    globalPeak: 0,
	globalPeakGPU: 0,
    _link: item.link,
    _displayValue: item.value,
  }))
})

// Parse value string to number (e.g., "1.5 MB" -> 1500000)
const parseValueToNumber = (value: string): number => {
  // Extract numeric part and unit
  const match = value.match(/^([\d.]+)\s*([KMGT]?B?)/)
  if (!match) return 0

  const num = parseFloat(match[1])
  const unit = match[2]

  const multipliers: Record<string, number> = {
    B: 1,
    KB: 1024,
    MB: 1024 * 1024,
    GB: 1024 * 1024 * 1024,
    TB: 1024 * 1024 * 1024 * 1024,
    K: 1000,
    M: 1000000,
    G: 1000000000,
    T: 1000000000000,
  }

  return num * (multipliers[unit] || 1)
}

// Get formatted value
const getFormattedValue = (func: FunctionStat): string => {
  return (func as any)._displayValue || func.total.alloc.sum.toString()
}

// Get numeric value for sorting/progress bar
const getValue = (func: FunctionStat): number => {
  return func.total.alloc.sum
}

// Get link for function
const getLinkForFunction = (func: FunctionStat): string => {
  return (func as any)._link || '#'
}
</script>

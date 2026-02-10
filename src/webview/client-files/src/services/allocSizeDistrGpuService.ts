/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/services/allocSizeDistrService.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
/**
 * Allocation Size Distribution API Service
 * Handles API calls for allocation size distribution data with Zod validation
 */

import { request } from '@/lib/request'
import {
  sizeMapDataSchema,
  scatterDataSchema,
  type SizeMapData,
  type ScatterData,
} from '@/types/alloc-size-distr'

export async function fetchSizeDistrGpu(): Promise<SizeMapData> {
  const data = await request<unknown>('/size-map-gpu.json', 'GET')
  return sizeMapDataSchema.parse(data)
}

export async function fetchScatterDataGpu(): Promise<ScatterData> {
  const data = await request<unknown>('/scatter.json', 'GET')
  return scatterDataSchema.parse(data)
}

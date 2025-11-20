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

export async function fetchSizeDistr(): Promise<SizeMapData> {
  const data = await request<unknown>('/size-map.json', 'GET')
  return sizeMapDataSchema.parse(data)
}

export async function fetchScatterData(): Promise<ScatterData> {
  const data = await request<unknown>('/scatter.json', 'GET')
  return scatterDataSchema.parse(data)
}

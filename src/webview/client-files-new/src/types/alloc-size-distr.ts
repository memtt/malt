/**
 * Types for Allocation Size Distribution analysis
 */

import { z } from 'zod'

// Zod schemas
export const sizeMapDataSchema = z.record(z.string(), z.number())

export const scatterPointSchema = z.object({
  x: z.number(),
  y: z.number(),
  v: z.number(),
})

export const scatterSeriesSchema = z.object({
  points: z.array(scatterPointSchema),
})

export const scatterDataSchema = z.object({
  sizeOverTime: scatterSeriesSchema,
  lifetimeOverSize: scatterSeriesSchema,
})

// Export types inferred from schemas
export type SizeMapData = z.infer<typeof sizeMapDataSchema>
export type ScatterPoint = z.infer<typeof scatterPointSchema>
export type ScatterSeries = z.infer<typeof scatterSeriesSchema>
export type ScatterData = z.infer<typeof scatterDataSchema>

export interface MostUsedSizeItem {
  size: number
  frequency: number
}

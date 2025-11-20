/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/types/flat.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
/**
 * Types for flat.json - Function statistics
 */

/**
 * Allocation/Free/Lifetime statistics
 */
export interface AllocStats {
  count: number
  min: number
  max: number
  sum: number
}

/**
 * Function memory statistics (own or total)
 */
export interface FunctionMemStats {
  countZeros: number
  maxAliveReq: number
  aliveReq: number
  alloc: AllocStats
  free: AllocStats
  lifetime: AllocStats
  globalPeak: number
  reallocCount: number
  reallocSumDelta: number
}

/**
 * Function entry from flat.json
 */
export interface FlatFunction {
  function: string
  functionShort: string
  line: number
  file: string
  own?: FunctionMemStats
  total: FunctionMemStats
}

/**
 * Metric configuration for extracting and formatting values
 */
export interface MetricConfig {
  name: string
  key: string
  extractor: (stats: FunctionMemStats) => number
  formatter: (value: number) => string
  defaultOrder: 'asc' | 'desc'
  refMode: 'sum' | 'max'
}

/**
 * Available metric types
 */
export type MetricType =
  | 'alloc.count'
  | 'alloc.sum'
  | 'alloc.min'
  | 'alloc.max'
  | 'alloc.mean'
  | 'free.count'
  | 'free.sum'
  | 'memops'
  | 'peakmem.local'
  | 'peakmem.global'
  | 'leaks'
  | 'lifetime.max'
  | 'lifetime.min'
  | 'recycling.ratio'
  | 'realloc.count'
  | 'realloc.sum'

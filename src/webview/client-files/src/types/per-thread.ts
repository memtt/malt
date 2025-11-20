/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/types/per-thread.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
/**
 * Per-Thread data types for thread-level allocation analysis
 */

/**
 * Statistics for a single allocation function
 */
export interface FunctionStats {
  count: number // Number of calls
  time: number // Time spent (in ticks)
  sum: number // Cumulated memory (in bytes)
}

/**
 * Statistics for all allocation functions in a single thread
 */
export interface ThreadStats {
  malloc: FunctionStats
  free: FunctionStats
  calloc: FunctionStats
  realloc: FunctionStats
  posix_memalign: FunctionStats
  aligned_alloc: FunctionStats
  memalign: FunctionStats
  valloc: FunctionStats
  pvalloc: FunctionStats
}

/**
 * System information
 */
export interface SystemInfo {
  ticksPerSecond: number // Conversion factor: ticks → seconds
}

/**
 * Main per-thread data structure from /data/summary.json
 */
export interface PerThreadData {
  system: SystemInfo
  threadStats: ThreadStats[] // Array of thread statistics (index = thread ID)
}

/**
 * Chart metric types
 */
export type ChartMetric = 'count' | 'time' | 'sum'

/**
 * Data point for bar chart
 */
export interface ThreadChartPoint {
  name: string // Thread name (e.g., "Thread 0")
  value: number // Metric value
  threadId: number // Original thread index
}

/**
 * Chart series for a single allocation function
 */
export interface ThreadChartSeries {
  key: string // Function name (e.g., "malloc")
  values: ThreadChartPoint[] // Array of points (one per thread)
  color?: string // Optional color for the series
}

/**
 * List of standard allocation functions
 */
export const ALLOC_FUNCTIONS = [
  'malloc',
  'free',
  'calloc',
  'realloc',
  'posix_memalign',
  'aligned_alloc',
  'memalign',
  'valloc',
  'pvalloc',
] as const

export type AllocFunction = (typeof ALLOC_FUNCTIONS)[number]

/**
 * Chart display mode
 */
export type ChartMode = 'stacked' | 'grouped'

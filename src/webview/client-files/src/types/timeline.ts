/**
 * Timeline data types for memory analysis over time
 */

/**
 * Single data point for D3 charts
 */
export interface ChartDataPoint {
  x: number
  y: number
}

/**
 * Chart series with multiple data points
 */
export interface ChartSeries {
  key: string // Series name/label
  values: ChartDataPoint[]
  color?: string // Optional color for the series
}

/**
 * Memory timeline structure containing allocation data over time
 * values[i] = [requested, physical, virtual, unknown, count]
 */
export interface TimelineMemory {
  values: number[][] // Array of [requested, physical, virtual, ?, count]
  perPoints: number // Time scale per point (in ticks)
}

/**
 * System memory timeline structure
 * values[i] = [free, swap, cached]
 */
export interface TimelineSystem {
  values: number[][] // Array of [free, swap, cached]
  perPoints: number // Time scale per point (in ticks)
}

/**
 * Memory bandwidth (allocation/free rates) over time
 * values[i] = [allocCount, allocSize, freeCount, freeSize]
 */
export interface TimelineBandwidth {
  values: number[][] // Array of [allocCount, allocSize, freeCount, freeSize]
  perPoints: number // Time scale per point (in ticks)
}

/**
 * Legacy format for allocation/free bandwidth
 */
export interface LegacyBandwidth {
  values: number[]
  scale: number // Time scale
  startTime: number
  endTime: number
}

/**
 * Main timeline data structure from /timed.json
 */
export interface TimelineData {
  ticksPerSecond: number // Conversion factor: ticks → seconds
  memoryTimeline: TimelineMemory
  systemTimeline: TimelineSystem
  memoryBandwidth: TimelineBandwidth
  // Legacy format (may not be present in all datasets)
  allocBandwidth?: LegacyBandwidth
  freeBandwidth?: LegacyBandwidth
  allocCnt?: LegacyBandwidth
  freeCnt?: LegacyBandwidth
}

/**
 * Stack frame in a call stack
 */
export interface StackFrame {
  function: string // Function name
  file?: string // Source file (optional)
  line?: number // Line number (optional)
}

/**
 * Allocated chunk with its call stack
 */
export interface AllocatedChunk {
  stack: StackFrame[] // Call stack trace
  chunkCount: number // Number of chunks
  chunkTotalSize: number // Total size in bytes
}

/**
 * Response from active chunks endpoint
 */
export interface ActiveChunksResponse {
  result: AllocatedChunk[]
  timestamp: number // Timestamp in ticks
}

/**
 * Chart data formatted for export (JSON/CSV)
 */
export interface ExportChartData {
  labels: string[] // Series labels
  ticksPerSecond: number
  data: ChartDataPoint[][] // Array of series data
}

/**
 * Memory series types for memoryTimeline
 */
export enum MemorySeriesIndex {
  Requested = 0,
  Physical = 1,
  Virtual = 2,
  Unknown = 3,
  Count = 4,
}

/**
 * System memory series types for systemTimeline
 */
export enum SystemSeriesIndex {
  Free = 0,
  Swap = 1,
  Cached = 2,
}

/**
 * Bandwidth series types for memoryBandwidth
 */
export enum BandwidthSeriesIndex {
  AllocCount = 0,
  AllocSize = 1,
  FreeCount = 2,
  FreeSize = 3,
}

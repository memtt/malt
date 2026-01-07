/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/types/stack-peaks.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
/**
 * Stack Peaks data types for stack memory analysis
 */

/**
 * Time profiler data for a single stack
 * Contains timestamps and corresponding maximum values
 */
export interface TimeProfiler {
  timestamp: number[] // Array of timestamps in ticks
  max: number[] // Array of maximum stack sizes at each timestamp
}

/**
 * Stack information for a single thread
 */
export interface StackInfo {
  size: number // Maximum stack size for this thread (in bytes)
  timeprofiler: TimeProfiler // Timeline of stack sizes over time
}

/**
 * Main stack peaks data structure from /stacks-mem.json
 */
export interface StackPeaksData {
  ticksPerSecond: number // Conversion factor: ticks → seconds
  stacks: StackInfo[] // Array of stack info, one per thread (index = thread ID)
}

/**
 * Function information for stack details
 */
export interface FunctionInfo {
  function: string // Function name
  file?: string // Source file path (optional)
  line?: number // Line number (optional)
}

/**
 * Stack detail for a single function frame
 */
export interface StackDetail {
  info: FunctionInfo // Function information
  mem: number // Memory used by this frame (in bytes)
}

/**
 * Stack details response from /stack.json?id={threadId}
 */
export interface StackDetailsData {
  details: StackDetail[] // Array of function frames with memory usage
}

/**
 * Chart data point for D3 visualization
 */
export interface StackChartDataPoint {
  x: number // X value (time or index)
  y: number // Y value (stack size in bytes)
}

/**
 * Chart series for line chart
 */
export interface StackChartSeries {
  key: string // Series name (e.g., "Thread 0")
  values: StackChartDataPoint[] // Array of data points
  color?: string // Optional color for the series
}

/**
 * Data for horizontal bar chart
 */
export interface BarChartData {
  label: string // Bar label (thread ID or function name)
  value: number // Bar value (size in bytes)
  index: number // Original index for click handling
  info?: FunctionInfo // Optional function info for detailed tooltip
}

/**
 * Summary statistics for Stack Peaks
 */
export interface StackPeaksSummary {
  largestStack: number // Largest stack size found (in bytes)
  threadsCount: number // Number of threads
}

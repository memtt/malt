/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 02/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/types/home.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
*    AUTHOR   : Sébastien Valat - 2026
***********************************************************/
/**
 * Home Page Types
 * Types for the home dashboard summary data
 */

/**
 * Profile information
 */
export interface ProfileInfo {
  /** Profile filename */
  filename: string;
}

/**
 * Run information
 */
export interface RunInfo {
  /** Executable path */
  exe: string;
  /** Command line */
  command: string;
  /** Tool name and version */
  tool: string;
  /** Hostname */
  hostname?: string;
  /** Execution date */
  date: string;
  /** Runtime in ticks */
  runtime: number;
  /** Allocator used */
  allocator?: string;
  /** List of custom allocator wrappers */
  allocatorWrappers: string[];
}

/**
 * System information
 */
export interface SystemInfo {
  /** CPU ticks per second */
  ticksPerSecond: number;
  /** Total physical memory */
  totalMemory: number;
  /** Free memory stats */
  freeMemory: {
    start: number;
    min: number;
    max: number;
  };
  /** Swap memory stats */
  swapMemory: {
    start: number;
    min: number;
    max: number;
  };
  /** Cached memory stats */
  cachedMemory: {
    start: number;
    min: number;
    max: number;
  };
}

/**
 * Summary statistics
 */
export interface SummaryStats {
  /** Peak physical memory */
  peakPhysicalMemory: number;
  /** Peak virtual memory */
  peakVirtualMemory: number;
  /** Peak requested memory */
  peakRequestedMemory: number;
  /** Peak internal memory used by MALT */
  peakInternalMemory: number;
  /** Peak segment count (alive objects) */
  peakSegmentCount: number;
  /** Total allocated memory (cumulated) */
  totalAllocatedMemory: number;
  /** Total allocation count */
  allocCount: number;
  /** Maximum allocation size */
  maxAllocSize: number;
  /** Mean allocation size */
  meanAllocSize: number;
  /** Minimum allocation size */
  minAllocSize: number;
  /** Peak allocation rate (bytes/sec) */
  peakAllocRate: number;
  /** Peak allocation call rate (calls/sec) */
  peakAllocCountRate: number;
  /** Recycling ratio */
  recyclingRatio: number;
  /** Leaked memory */
  leakedMem: number;
  /** Leaked object count */
  leakedCount: number;
  /** Largest stack size */
  largestStack: number;
  /** Global variables memory */
  globalVarMem: number;
  /** TLS variables memory */
  tlsVarMem?: number;
  /** Number of global variables */
  numGblVar?: number;
  /** Max number of threads at same time */
  maxThreadCount?: number;
  /** Memory peak on the GPU side */
  peakGpuMemory?: number;
  /** Number of GPU allocation */
  gpuAllocCount?: number;
  /** Total amount of memory allocated on the GPU during the run */
  gpuAllocSum?: number;
}

/**
 * Summary domains (C, Python, etc.)
 */
export interface SummaryDomains {
  /** Allocation counts by domain */
  counters: {
    c: number;
    mmap: number;
    pyObj: number;
    pyMem: number;
    pyRaw: number;
	gpu: number;
  };
  /** Memory usage by domain */
  mem: {
    c: number;
    mmap: number;
    pyObj: number;
    pyMem: number;
    pyRaw: number;
	gpu: number;
  };
}

/**
 * Summary warnings (key -> array of warning messages)
 */
export interface SummaryWarnings {
  [key: string]: string[];
}

/**
 * Home summary data from backend
 */
export interface HomeSummaryData {
  /** Profile information */
  profile: ProfileInfo;
  /** Run information */
  run: RunInfo;
  /** System information */
  system: SystemInfo;
  /** Summary statistics */
  summary: SummaryStats;
  /** Summary domains (optional, may not exist in old versions) */
  summaryDomains?: SummaryDomains;
  /** Summary warnings */
  summaryWarnings: SummaryWarnings;
}

/**
 * Summary view entry configuration
 */
export interface SummaryViewEntry {
  /** Type of entry (normal or separator) */
  type?: 'separator';
  /** Key in summary stats */
  key?: keyof SummaryStats;
  /** Display name */
  name?: string;
  /** Display level (1 = always show, 2 = show on details) */
  level: number;
  /** Help text */
  help?: string;
  /** Format function */
  format?: (value: number) => string;
}

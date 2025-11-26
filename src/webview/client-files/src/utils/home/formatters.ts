/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/utils/home/formatters.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
/**
 * Home page formatters and utilities
 */

import { humanReadable } from '@/lib/helpers';
import type { SummaryViewEntry } from '@/types/home';

/**
 * Convert ticks to HH:MM:SS format
 */
export function ticksToHourMinSec(ticks: number, ticksPerSecond: number): string {
  const seconds = Math.floor(ticks / ticksPerSecond);
  const hours = Math.floor(seconds / 3600);
  const minutes = Math.floor((seconds % 3600) / 60);
  const secs = seconds % 60;

  const pad = (n: number) => String(n).padStart(2, '0');

  return `${pad(hours)}:${pad(minutes)}:${pad(secs)}`;
}

/**
 * Format execution time
 */
export function formatExecTime(runtime: number, ticksPerSecond: number): string {
  return ticksToHourMinSec(runtime, ticksPerSecond);
}

/**
 * Format CPU frequency
 */
export function formatCpuFreq(ticksPerSecond: number): string {
  return humanReadable(ticksPerSecond, 1, 'Hz', true);
}

/**
 * Summary view entries configuration
 * Matches Angular summaryViewEntries
 */
export const summaryViewEntries: SummaryViewEntry[] = [
  {
    key: 'peakPhysicalMemory',
    name: 'Physical memory peak',
    level: 1,
    help: 'Peak of total physical memory obtained by tracking /proc/self/statm. It contain all allocated objects, global variables and binary code.',
    format: (x) => humanReadable(x, 1, 'B', true),
  },
  {
    key: 'peakVirtualMemory',
    name: 'Virtual memory peak',
    level: 1,
    help: 'Peak of total virtual memory obtained by tracking /proc/self/statm. It contain all allocated objects, global variables and binary code.',
    format: (x) => humanReadable(x, 1, 'B', true),
  },
  {
    key: 'peakRequestedMemory',
    name: 'Requested memory peak',
    level: 1,
    help: 'Peak of requested memory managed by malloc, free... It contain all dynamically allocated objects.',
    format: (x) => humanReadable(x, 1, 'B', true),
  },
  {
    key: 'peakSegmentCount',
    name: 'Alive object peak',
    level: 2,
    help: 'Peak number of alive object allocated by malloc, calloc, realloc....',
    format: (x) => humanReadable(x, 1, '', true),
  },
  {
    key: 'peakInternalMemory',
    name: 'MALT peak memory',
    level: 2,
    help: 'Peak of memory internally used by MALT.',
    format: (x) => humanReadable(x, 1, 'B', true),
  },
  {
    type: 'separator',
    level: 2,
  },
  {
    key: 'totalAllocatedMemory',
    name: 'Cumulated memory allocations',
    level: 1,
    help: 'Sum of all dynamic memory allocation througth malloc, calloc, realloc....',
    format: (x) => humanReadable(x, 1, 'B', true),
  },
  {
    key: 'allocCount',
    name: 'Allocation count',
    level: 1,
    help: 'Total number of all dynamic memory allocation with malloc, calloc, realloc....',
    format: (x) => humanReadable(x, 1, '', true),
  },
  {
    key: 'recyclingRatio',
    name: 'Recycling ratio',
    level: 1,
    help: 'Ratio of cumulated memory allocations over maximum requested memory. Ideally it might be 1 if the program allocate the memory on start and free on end.',
    format: (x) => x.toFixed(1),
  },
  {
    type: 'separator',
    level: 2,
  },
  {
    key: 'minAllocSize',
    name: 'Smallest allocations',
    level: 2,
    help: 'The smallest request size received by malloc, calloc...',
    format: (x) => humanReadable(x, 1, 'B', true),
  },
  {
    key: 'meanAllocSize',
    name: 'Mean allocations',
    level: 2,
    help: 'The mean request size received by malloc, calloc...',
    format: (x) => humanReadable(x, 1, 'B', true),
  },
  {
    key: 'maxAllocSize',
    name: 'Largest allocations',
    level: 2,
    help: 'The largest request size received by malloc, calloc...',
    format: (x) => humanReadable(x, 1, 'B', true),
  },
  {
    type: 'separator',
    level: 2,
  },
  {
    key: 'leakedMem',
    name: 'Leaked memory',
    level: 1,
    help: 'Total memory which has leaked (malloc, calloc... without free).',
    format: (x) => humanReadable(x, 1, 'B', true),
  },
  {
    key: 'leakedCount',
    name: 'Leaked object count',
    level: 2,
    help: 'Total number of allocated object which have leaked (malloc, calloc... without free).',
    format: (x) => humanReadable(x, 1, '', true),
  },
  {
    type: 'separator',
    level: 2,
  },
  {
    key: 'largestStack',
    name: 'Largest stack',
    level: 1,
    help: 'Memory used by the largest stack seen during execution of threads.',
    format: (x) => humanReadable(x, 1, 'B', true),
  },
  {
    key: 'globalVarMem',
    name: 'Global variables',
    level: 1,
    help: 'Memory used by global variables.',
    format: (x) => humanReadable(x, 1, 'B', true),
  },
  {
    key: 'tlsVarMem',
    name: 'TLS variables',
    level: 1,
    help: 'Memory used by TLS variables (already multiplied by number of thread of execution).',
    format: (x) => humanReadable(x, 1, 'B', true),
  },
  {
    key: 'maxThreadCount',
    name: 'Number of threads',
    level: 1,
    help: 'Maximium number of thread running at same time.',
    format: (x) => humanReadable(x, 1, '', false),
  },
  {
    key: 'numGblVar',
    name: 'Global variable count',
    level: 2,
    help: 'Total number of global and TLS variables from your application and libraries.',
    format: (x) => humanReadable(x, 1, '', true),
  },
  {
    type: 'separator',
    level: 2,
  },
  {
    key: 'peakAllocRate',
    name: 'Peak allocation rate',
    level: 1,
    help: 'Peak of memory allocation rate (in memory size).',
    format: (x) => humanReadable(x, 1, 'B/s', true),
  },
  {
    key: 'peakAllocCountRate',
    name: 'Peak allocation call rate',
    level: 2,
    help: 'Peak of memory allocation call rate.',
    format: (x) => humanReadable(x, 1, '/s', true),
  },
];

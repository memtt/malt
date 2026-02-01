/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/types/sources.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/
/**
 * Types for Sources page - code editor and analysis
 */

import { z } from 'zod';

// ============================================================================
// Stack Info - Memory statistics for allocations/frees
// ============================================================================

export const stackMinMaxInfoSchema = z.object({
  count: z.number(),
  sum: z.number(),
  min: z.number(),
  max: z.number(),
});

export const stackInfoDataSchema = z.object({
  countZeros: z.number(),
  maxAliveReq: z.number(),
  aliveReq: z.number(),
  maxAliveReqGPU: z.number(),
  aliveReqGPU: z.number(),
  alloc: stackMinMaxInfoSchema,
  free: stackMinMaxInfoSchema,
  gpuAlloc: stackMinMaxInfoSchema,
  gpuFree: stackMinMaxInfoSchema,
  lifetime: stackMinMaxInfoSchema,
  mmap: stackMinMaxInfoSchema,
  munmap: stackMinMaxInfoSchema,
  // Additional fields that exist in backend data
  globalPeak: z.number().optional(),
  globalPeakGPU: z.number().optional(),
  reallocCount: z.number().optional(),
  reallocSumDelta: z.number().optional(),
});

export type StackMinMaxInfo = z.infer<typeof stackMinMaxInfoSchema>;
export type StackInfoData = z.infer<typeof stackInfoDataSchema>;

// ============================================================================
// Source Annotations - Per-line profiling data
// ============================================================================

export const sourceAnnotationSchema = z.object({
  line: z.number(),
  file: z.string().optional(),
  function: z.string().optional(),
  own: stackInfoDataSchema.optional(),
  childs: stackInfoDataSchema.optional(),
  total: stackInfoDataSchema.optional(),
});

export type SourceAnnotation = z.infer<typeof sourceAnnotationSchema>;

export const sourceAnnotationsArraySchema = z.array(sourceAnnotationSchema);

// ============================================================================
// Function List - Flat profile of functions
// ============================================================================

export const functionStatSchema = z.object({
  function: z.string(),
  functionShort: z.string().optional(),
  file: z.string().optional(),
  line: z.number().optional(),
  own: stackInfoDataSchema.optional(),
  total: stackInfoDataSchema,
  // Additional fields for realloc/globalPeak
  reallocCount: z.number().optional(),
  reallocSumDelta: z.number().optional(),
  globalPeak: z.number().optional(),
});

export type FunctionStat = z.infer<typeof functionStatSchema>;

export const functionStatsArraySchema = z.array(functionStatSchema);

// ============================================================================
// Call Stack Tree - Hierarchical stack traces
// ============================================================================

export const locationSchema = z.object({
  function: z.string(),
  file: z.string().optional(),
  line: z.number().optional(),
});

export const callStackEntrySchema = z.object({
  stackId: z.number(),
  stackDepth: z.number(),
  parentStackId: z.number(),
  parentStackDepth: z.number(),
  location: locationSchema,
  infos: stackInfoDataSchema,
  hasChild: z.union([z.boolean(), z.number()]).transform((val) => Boolean(val)),
});

export type Location = z.infer<typeof locationSchema>;
export type CallStackEntry = z.infer<typeof callStackEntrySchema>;

export const callStackArraySchema = z.array(callStackEntrySchema);

// ============================================================================
// Metric Selector - Configuration for metric selection
// ============================================================================

export type MetricKey =
  | 'alloc.sum'
  | 'alloc.count'
  | 'alloc.min'
  | 'alloc.mean'
  | 'alloc.max'
  | 'free.sum'
  | 'free.count'
  | 'memops'
  | 'peakmem.local'
  | 'peakmem.global'
  | 'leaks'
  | 'lifetime.max'
  | 'lifetime.min'
  | 'recycling.ratio'
  | 'realloc.count'
  | 'realloc.sum'
  | 'mmap.sum'
  | 'mmap.count'
  | 'mmap.min'
  | 'mmap.mean'
  | 'mmap.max'
  | 'munmap.sum'
  | 'munmap.count'
  | 'munmap.min'
  | 'munmap.mean'
  | 'munmap.max'
  | 'gpuAlloc.sum'
  | 'gpuAlloc.count'
  | 'peakmemGPU.local'
  | 'peakmemGPU.global';
  

export type MetricOrder = 'asc' | 'desc';

export interface MetricSelectorState {
  metric: MetricKey;
  inclusive: boolean;
  ratio: boolean;
  order: MetricOrder;
  query: string;
  currentPage: number;
  perPage: number;
  totalElements: number;
}

// ============================================================================
// Call Stack Filter - Filter for call stack queries
// ============================================================================

export interface CallStackFilter {
  file?: string;
  line?: number;
  function?: string;
}

// ============================================================================
// Request/Response types for API calls
// ============================================================================

export interface SourceFileRequest {
  path: string;
}

export interface FileInfosRequest {
  file: string;
}

export interface CallStackLevelRequest {
  parentStackId: number;
  parentStackDepth: number;
  filter: CallStackFilter;
}

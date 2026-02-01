/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/utils/metrics/funcMetrics.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/
/**
 * Function Metrics Configuration
 * Ported from Angular func-metrics.js
 */

import { humanReadable } from '@/lib/helpers'
import type { MetricDefinition, MetricListItem } from '@/types/metrics'
import type { MetricKey } from '@/types/sources'

/**
 * All available metrics for function profiling
 */
export const FUNC_METRICS: Record<MetricKey, MetricDefinition> = {
  'alloc.sum': {
    name: 'Allocated mem.',
    key: 'alloc.sum',
    extractor: (x) => x.alloc.sum,
    formatter: (x) => humanReadable(x, 1, 'B', true),
    defaultOrder: 'desc',
    ref: 'sum',
  },
  'alloc.count': {
    name: 'Allocated count',
    key: 'alloc.count',
    extractor: (x) => x.alloc.count,
    formatter: (x) => humanReadable(x, 1, '', true),
    defaultOrder: 'desc',
    ref: 'sum',
  },
  'alloc.min': {
    name: 'Min. alloc size',
    key: 'alloc.min',
    extractor: (x) => x.alloc.min,
    formatter: (x) => humanReadable(x, 1, 'B', true),
    defaultOrder: 'asc',
    ref: 'max',
  },
  'alloc.mean': {
    name: 'Mean alloc size',
    key: 'alloc.mean',
    extractor: (x) => (x.alloc.count === 0 ? 0 : x.alloc.sum / x.alloc.count),
    formatter: (x) => humanReadable(x, 1, 'B', true),
    defaultOrder: 'asc',
    ref: 'max',
  },
  'alloc.max': {
    name: 'Max. alloc size',
    key: 'alloc.max',
    extractor: (x) => x.alloc.max,
    formatter: (x) => humanReadable(x, 1, 'B', true),
    defaultOrder: 'desc',
    ref: 'max',
  },
  'free.sum': {
    name: 'Freed mem.',
    key: 'free.sum',
    extractor: (x) => x.free.sum,
    formatter: (x) => humanReadable(x, 1, 'B', true),
    defaultOrder: 'desc',
    ref: 'sum',
  },
  'free.count': {
    name: 'Free count',
    key: 'free.count',
    extractor: (x) => x.free.count,
    formatter: (x) => humanReadable(x, 1, '', true),
    defaultOrder: 'desc',
    ref: 'sum',
  },
  memops: {
    name: 'Memory ops.',
    key: 'memops',
    extractor: (x) => x.alloc.count + x.free.count,
    formatter: (x) => humanReadable(x, 1, '', true),
    defaultOrder: 'desc',
    ref: 'sum',
  },
  'peakmem.local': {
    name: 'Local peak',
    key: 'peakmem.local',
    extractor: (x) => x.maxAliveReq,
    formatter: (x) => humanReadable(x, 1, 'B', true),
    defaultOrder: 'desc',
    ref: 'max',
  },
  'peakmem.global': {
    name: 'Global peak',
    key: 'peakmem.global',
    extractor: (x) => x.globalPeak ?? 0,
    formatter: (x) => humanReadable(x, 1, 'B', true),
    defaultOrder: 'desc',
    ref: 'sum',
  },
  'peakmemGPU.local': {
    name: 'GPU Local peak',
    key: 'peakmemGPU.local',
    extractor: (x) => x.maxAliveReqGPU,
    formatter: (x) => humanReadable(x, 1, 'B', true),
    defaultOrder: 'desc',
    ref: 'max',
  },
  'peakmemGPU.global': {
    name: 'GPU Global peak',
    key: 'peakmemGPU.global',
    extractor: (x) => x.globalPeakGPU ?? 0,
    formatter: (x) => humanReadable(x, 1, 'B', true),
    defaultOrder: 'desc',
    ref: 'sum',
  },
  leaks: {
    name: 'Leaks',
    key: 'leaks',
    extractor: (x) => x.aliveReq,
    formatter: (x) => humanReadable(x, 1, 'B', true),
    defaultOrder: 'desc',
    ref: 'sum',
  },
  'lifetime.max': {
    name: 'Max lifetime',
    key: 'lifetime.max',
    extractor: (x) => x.lifetime.max,
    formatter: (x) => humanReadable(x, 1, '', true),
    defaultOrder: 'desc',
    ref: 'max',
  },
  'lifetime.min': {
    name: 'Min lifetime',
    key: 'lifetime.min',
    extractor: (x) => x.lifetime.min,
    formatter: (x) => humanReadable(x, 1, '', true),
    defaultOrder: 'asc',
    ref: 'max',
  },
  'recycling.ratio': {
    name: 'Recycling ratio',
    key: 'recycling.ratio',
    extractor: (x) => (x.alloc.count === 0 ? 1 : x.alloc.sum / x.maxAliveReq),
    formatter: (x) => humanReadable(x, 1, '', true),
    defaultOrder: 'desc',
    ref: 'max',
  },
  'realloc.count': {
    name: 'Realloc count',
    key: 'realloc.count',
    extractor: (x) => x.reallocCount ?? 0,
    formatter: (x) => humanReadable(x, 1, '', true),
    defaultOrder: 'desc',
    ref: 'sum',
  },
  'realloc.sum': {
    name: 'Realloc sum',
    key: 'realloc.sum',
    extractor: (x) => x.reallocSumDelta ?? 0,
    formatter: (x) => humanReadable(x, 1, '', true),
    defaultOrder: 'desc',
    ref: 'sum',
  },
    'mmap.sum': {
    name: 'Mmap mem.',
    key: 'mmap.sum',
    extractor: (x) => x.mmap.sum,
    formatter: (x) => humanReadable(x),
    defaultOrder: 'desc',
    ref: 'sum',
  },
  'mmap.count': {
    name: 'Mmap count',
    key: 'mmap.count',
    extractor: (x) => x.mmap.count,
    formatter: (x) => humanReadable(x, 1, '', true),
    defaultOrder: 'desc',
    ref: 'sum',
  },
  'mmap.min': {
    name: 'Min. mmap size',
    key: 'mmap.min',
    extractor: (x) => x.mmap.min,
    formatter: (x) => humanReadable(x),
    defaultOrder: 'asc',
    ref: 'max',
  },
  'mmap.mean': {
    name: 'Mean mmap size',
    key: 'mmap.mean',
    extractor: (x) => (x.mmap.count === 0 ? 0 : x.mmap.sum / x.mmap.count),
    formatter: (x) => humanReadable(x),
    defaultOrder: 'asc',
    ref: 'max',
  },
  'mmap.max': {
    name: 'Max. mmap size',
    key: 'mmap.max',
    extractor: (x) => x.mmap.max,
    formatter: (x) => humanReadable(x),
    defaultOrder: 'desc',
    ref: 'max',
  },
  'munmap.sum': {
    name: 'Munmap mem.',
    key: 'munmap.sum',
    extractor: (x) => x.munmap.sum,
    formatter: (x) => humanReadable(x),
    defaultOrder: 'desc',
    ref: 'sum',
  },
  'munmap.count': {
    name: 'Munmap count',
    key: 'munmap.count',
    extractor: (x) => x.munmap.count,
    formatter: (x) => humanReadable(x, 1, '', true),
    defaultOrder: 'desc',
    ref: 'sum',
  },
  'munmap.min': {
    name: 'Min. munmap size',
    key: 'munmap.min',
    extractor: (x) => x.munmap.min,
    formatter: (x) => humanReadable(x),
    defaultOrder: 'asc',
    ref: 'max',
  },
  'munmap.mean': {
    name: 'Mean munmap size',
    key: 'munmap.mean',
    extractor: (x) => (x.munmap.count === 0 ? 0 : x.munmap.sum / x.munmap.count),
    formatter: (x) => humanReadable(x),
    defaultOrder: 'asc',
    ref: 'max',
  },
  'munmap.max': {
    name: 'Max. munmap size',
    key: 'munmap.max',
    extractor: (x) => x.munmap.max,
    formatter: (x) => humanReadable(x),
    defaultOrder: 'desc',
    ref: 'max',
  },
  'gpuAlloc.sum': {
    name: 'GPU Allocated mem.',
    key: 'gpuAlloc.sum',
    extractor: (x) => x.gpuAlloc.sum,
    formatter: (x) => humanReadable(x, 1, 'B', true),
    defaultOrder: 'desc',
    ref: 'sum',
  },
  'gpuAlloc.count': {
    name: 'GPU Allocated count',
    key: 'gpuAlloc.count',
    extractor: (x) => x.gpuAlloc.count,
    formatter: (x) => humanReadable(x, 1, '', true),
    defaultOrder: 'desc',
    ref: 'sum',
  },
}

/**
 * Metric categories for organized dropdown
 */
export interface MetricCategory {
  label: string
  metrics: MetricListItem[]
}

/**
 * Metrics organized by category
 */
const METRIC_CATEGORIES: Record<string, MetricKey[]> = {
  'Bloc sizes': [
    'alloc.sum', // Allocated memory
    'alloc.count', // Allocated count
    'alloc.min', // Min. allocated size
    'alloc.mean', // Mean allocated size
    'alloc.max', // Max. allocated size
  ],
  Frees: [
    'free.sum', // Free memory
    'free.count', // Free count
  ],
  Peaks: [
    'peakmem.local', // Local peak
    'peakmem.global', // Global peak
  ],
  Leaks: [
    'leaks', // Leaks
  ],
  'Short life allocations': [
    'lifetime.max', // Max. lifetime
    'lifetime.min', // Min. lifetime
    'recycling.ratio', // Recycling ratio
  ],
  Realloc: [
    'realloc.count', // Realloc count
    'realloc.sum', // Realloc sum
  ],
  Mmap: [
    'mmap.sum', // Allocated memory
    'mmap.count', // Allocated count
    'mmap.min', // Min. allocated size
    'mmap.mean', // Mean allocated size
    'mmap.max', // Max. allocated size
  ],
  Munmap: [
    'munmap.sum', // Allocated memory
    'munmap.count', // Allocated count
    'munmap.min', // Min. allocated size
    'munmap.mean', // Mean allocated size
    'munmap.max', // Max. allocated size
  ],
  GPU: [
    'gpuAlloc.sum', // Allocated memory
    'gpuAlloc.count', // Allocated count
	'peakmemGPU.local', // Allocated memory
    'peakmemGPU.global', // Allocated count
  ]
}

/**
 * Get list of all metrics organized by category for dropdown
 */
export function getMetricList(): MetricCategory[] {
  return Object.entries(METRIC_CATEGORIES).map(([label, keys]) => ({
    label,
    metrics: keys.map((key) => ({
      name: FUNC_METRICS[key].name,
      key: FUNC_METRICS[key].key,
    })),
  }))
}

/**
 * Get metric definition by key
 */
export function getMetric(key: MetricKey): MetricDefinition {
  return FUNC_METRICS[key]
}

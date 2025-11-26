/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/utils/flat/metrics.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
/**
 * Function metrics utilities
 * Ported from Angular func-metrics.js
 */

import { humanReadable } from '@/lib/helpers'
import type { MetricConfig, MetricType, FunctionMemStats } from '@/types/flat'

/**
 * Metric configurations
 */
export const METRICS: Record<MetricType, MetricConfig> = {
  'alloc.sum': {
    name: 'Allocated mem.',
    key: 'alloc.sum',
    extractor: (x) => x.alloc.sum,
    formatter: (x) => humanReadable(x),
    defaultOrder: 'desc',
    refMode: 'sum',
  },
  'alloc.count': {
    name: 'Allocated count',
    key: 'alloc.count',
    extractor: (x) => x.alloc.count,
    formatter: (x) => humanReadable(x, 1, '', true),
    defaultOrder: 'desc',
    refMode: 'sum',
  },
  'alloc.min': {
    name: 'Min. alloc size',
    key: 'alloc.min',
    extractor: (x) => x.alloc.min,
    formatter: (x) => humanReadable(x),
    defaultOrder: 'asc',
    refMode: 'max',
  },
  'alloc.mean': {
    name: 'Mean alloc size',
    key: 'alloc.mean',
    extractor: (x) => (x.alloc.count === 0 ? 0 : x.alloc.sum / x.alloc.count),
    formatter: (x) => humanReadable(x),
    defaultOrder: 'asc',
    refMode: 'max',
  },
  'alloc.max': {
    name: 'Max. alloc size',
    key: 'alloc.max',
    extractor: (x) => x.alloc.max,
    formatter: (x) => humanReadable(x),
    defaultOrder: 'desc',
    refMode: 'max',
  },
  'free.sum': {
    name: 'Freed mem.',
    key: 'free.sum',
    extractor: (x) => x.free.sum,
    formatter: (x) => humanReadable(x),
    defaultOrder: 'desc',
    refMode: 'sum',
  },
  'free.count': {
    name: 'Free count',
    key: 'free.count',
    extractor: (x) => x.free.count,
    formatter: (x) => humanReadable(x, 1, '', true),
    defaultOrder: 'desc',
    refMode: 'sum',
  },
  memops: {
    name: 'Memory ops.',
    key: 'memops',
    extractor: (x) => x.alloc.count + x.free.count,
    formatter: (x) => humanReadable(x, 1, '', true),
    defaultOrder: 'desc',
    refMode: 'sum',
  },
  'peakmem.local': {
    name: 'Local peak',
    key: 'peakmem.local',
    extractor: (x) => x.maxAliveReq,
    formatter: (x) => humanReadable(x),
    defaultOrder: 'desc',
    refMode: 'max',
  },
  'peakmem.global': {
    name: 'Global peak',
    key: 'peakmem.global',
    extractor: (x) => x.globalPeak,
    formatter: (x) => humanReadable(x),
    defaultOrder: 'desc',
    refMode: 'sum',
  },
  leaks: {
    name: 'Leaks',
    key: 'leaks',
    extractor: (x) => x.aliveReq,
    formatter: (x) => humanReadable(x),
    defaultOrder: 'desc',
    refMode: 'sum',
  },
  'lifetime.max': {
    name: 'Max lifetime',
    key: 'lifetime.max',
    extractor: (x) => x.lifetime.max,
    formatter: (x) => humanReadable(x, 1, '', true),
    defaultOrder: 'desc',
    refMode: 'max',
  },
  'lifetime.min': {
    name: 'Min lifetime',
    key: 'lifetime.min',
    extractor: (x) => x.lifetime.min,
    formatter: (x) => humanReadable(x, 1, '', true),
    defaultOrder: 'asc',
    refMode: 'max',
  },
  'recycling.ratio': {
    name: 'Recycling ratio',
    key: 'recycling.ratio',
    extractor: (x) => (x.alloc.count === 0 ? 1 : x.alloc.sum / x.maxAliveReq),
    formatter: (x) => x.toFixed(1),
    defaultOrder: 'desc',
    refMode: 'max',
  },
  'realloc.count': {
    name: 'Realloc count',
    key: 'realloc.count',
    extractor: (x) => x.reallocCount,
    formatter: (x) => humanReadable(x, 1, '', true),
    defaultOrder: 'desc',
    refMode: 'sum',
  },
  'realloc.sum': {
    name: 'Realloc sum',
    key: 'realloc.sum',
    extractor: (x) => x.reallocSumDelta,
    formatter: (x) => humanReadable(x, 1, '', true),
    defaultOrder: 'desc',
    refMode: 'sum',
  },
  'mmap.sum': {
    name: 'Mmap mem.',
    key: 'mmap.sum',
    extractor: (x) => x.mmap.sum,
    formatter: (x) => humanReadable(x),
    defaultOrder: 'desc',
    refMode: 'sum',
  },
  'mmap.count': {
    name: 'Mmap count',
    key: 'mmap.count',
    extractor: (x) => x.mmap.count,
    formatter: (x) => humanReadable(x, 1, '', true),
    defaultOrder: 'desc',
    refMode: 'sum',
  },
  'mmap.min': {
    name: 'Min. mmap size',
    key: 'mmap.min',
    extractor: (x) => x.mmap.min,
    formatter: (x) => humanReadable(x),
    defaultOrder: 'asc',
    refMode: 'max',
  },
  'mmap.mean': {
    name: 'Mean mmap size',
    key: 'mmap.mean',
    extractor: (x) => (x.mmap.count === 0 ? 0 : x.mmap.sum / x.mmap.count),
    formatter: (x) => humanReadable(x),
    defaultOrder: 'asc',
    refMode: 'max',
  },
  'mmap.max': {
    name: 'Max. mmap size',
    key: 'mmap.max',
    extractor: (x) => x.mmap.max,
    formatter: (x) => humanReadable(x),
    defaultOrder: 'desc',
    refMode: 'max',
  },
  'munmap.sum': {
    name: 'Munmap mem.',
    key: 'munmap.sum',
    extractor: (x) => x.munmap.sum,
    formatter: (x) => humanReadable(x),
    defaultOrder: 'desc',
    refMode: 'sum',
  },
  'munmap.count': {
    name: 'Munmap count',
    key: 'munmap.count',
    extractor: (x) => x.munmap.count,
    formatter: (x) => humanReadable(x, 1, '', true),
    defaultOrder: 'desc',
    refMode: 'sum',
  },
  'munmap.min': {
    name: 'Min. munmap size',
    key: 'munmap.min',
    extractor: (x) => x.munmap.min,
    formatter: (x) => humanReadable(x),
    defaultOrder: 'asc',
    refMode: 'max',
  },
  'munmap.mean': {
    name: 'Mean munmap size',
    key: 'munmap.mean',
    extractor: (x) => (x.munmap.count === 0 ? 0 : x.munmap.sum / x.munmap.count),
    formatter: (x) => humanReadable(x),
    defaultOrder: 'asc',
    refMode: 'max',
  },
  'munmap.max': {
    name: 'Max. munmap size',
    key: 'munmap.max',
    extractor: (x) => x.munmap.max,
    formatter: (x) => humanReadable(x),
    defaultOrder: 'desc',
    refMode: 'max',
  },
}

/**
 * Get metric value from function stats
 */
export function getMetricValue(
  stats: FunctionMemStats | undefined,
  metric: MetricType,
): number {
  if (!stats) return 0
  return METRICS[metric].extractor(stats)
}

/**
 * Get formatted metric value
 */
export function getFormattedMetricValue(
  stats: FunctionMemStats | undefined,
  metric: MetricType,
): string {
  const value = getMetricValue(stats, metric)
  return METRICS[metric].formatter(value)
}

/**
 * Compute reference value for a metric across all functions
 * Used for calculating percentage in progress bars
 */
export function computeMetricRef(
  stats: FunctionMemStats[],
  metric: MetricType,
): number {
  const metricConfig = METRICS[metric]
  let result = 0

  switch (metricConfig.refMode) {
    case 'sum':
      for (const stat of stats) {
        result += getMetricValue(stat, metric)
      }
      break
    case 'max':
      for (const stat of stats) {
        const value = getMetricValue(stat, metric)
        if (value > result) {
          result = value
        }
      }
      break
  }

  return result
}

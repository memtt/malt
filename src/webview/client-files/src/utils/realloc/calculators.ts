/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/utils/realloc/calculators.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
/**
 * Realloc Calculators
 * Pure functions for calculations
 */

import type { ReallocData, DeltaDistribution, Log2HistoEntry, MostUsedEntry } from '@/types/realloc';
import { humanReadable } from '@/lib/helpers';

/**
 * Find the most used realloc entry
 */
export function findMostUsed(data: ReallocData): MostUsedEntry | null {
  if (data.length === 0) return null;

  const sorted = [...data].sort((a, b) => b.count - a.count);
  const entry = sorted[0];

  return {
    ...entry,
    label: `${entry.count} of ${humanReadable(entry.oldSize, 1, 'B', true)} → ${humanReadable(entry.newSize, 1, 'B', true)}`
  };
}

/**
 * Calculate mean delta between oldSize and newSize
 */
export function calculateMeanDelta(data: ReallocData): number {
  if (data.length === 0) return 0;

  const sum = data.reduce((acc, item) =>
    acc + Math.abs(item.newSize - item.oldSize), 0
  );

  return sum / data.length;
}

/**
 * Calculate total count of realloc operations
 */
export function calculateTotalCount(data: ReallocData): number {
  return data.reduce((acc, item) => acc + item.count, 0);
}

/**
 * Generate delta distribution
 */
export function generateDeltaDistribution(data: ReallocData): DeltaDistribution {
  const res: DeltaDistribution = {};

  data.forEach(item => {
    if (item.newSize !== 0 && item.oldSize !== 0) {
      const delta = item.newSize - item.oldSize;
      if (delta !== 0) {
        res[delta] = (res[delta] || 0) + item.count;
      }
    }
  });

  return res;
}

/**
 * Build Log2 histogram from delta distribution
 */
export function buildLog2Histogram(deltaDistribution: DeltaDistribution): Log2HistoEntry[] {
  const tmp: Record<number, number> = {};

  Object.entries(deltaDistribution).forEach(([delta, count]) => {
    const deltaNum = Number(delta);
    const id = Math.ceil(Math.log(Math.abs(deltaNum)) / Math.log(2));
    const key = deltaNum < 0 ? -id : id;
    tmp[key] = (tmp[key] || 0) + count;
  });

  return Object.entries(tmp)
    .map(([sizeLog, count]) => {
      const sizeLogNum = Number(sizeLog);
      const isNegative = sizeLogNum < 0;
      const absSizeLog = Math.abs(sizeLogNum);

      const min = Math.pow(2, absSizeLog - 1);
      const max = Math.pow(2, absSizeLog);

      const minLabel = humanReadable(isNegative ? -max : min, 1, 'B', true);
      const maxLabel = humanReadable(isNegative ? -min : max, 1, 'B', true);

      return {
        sizeLog: sizeLogNum,
        count,
        label: `${minLabel} - ${maxLabel}`
      };
    })
    .sort((a, b) => a.sizeLog - b.sizeLog);
}

/**
 * Get top N most used realloc entries with "Others" aggregate
 */
export function getTopMostUsed(data: ReallocData, limit: number = 20): MostUsedEntry[] {
  const sorted = [...data].sort((a, b) => b.count - a.count);

  const top = sorted.slice(0, limit);
  const others = sorted.slice(limit);

  const result: MostUsedEntry[] = [];

  // Add top entries
  result.push(...top.map(entry => ({
    ...entry,
    label: `${humanReadable(entry.oldSize, 1, 'B', true)} → ${humanReadable(entry.newSize, 1, 'B', true)}`
  })));

  // Add "others" entry at the end if there are remaining items
  if (others.length > 0) {
    const othersCount = others.reduce((acc, item) => acc + item.count, 0);
    result.push({
      oldSize: 0,
      newSize: 0,
      count: othersCount,
      label: 'others'
    });
  }

  // Sort by count descending to match Angular behavior
  return result.sort((a, b) => b.count - a.count);
}

/**
 * Get top deltas with threshold filtering and "Others" aggregate
 */
export function getTopDeltas(deltaDistribution: DeltaDistribution, limit: number = 50) {
  const deltas = Object.entries(deltaDistribution)
    .map(([delta, count]) => ({ delta: Number(delta), count }));

  const maxCount = Math.max(...deltas.map(d => d.count));
  const threshold = maxCount / 1000;

  // Sort by frequency (count) descending
  const sortedByFrequency = deltas
    .filter(d => d.count > threshold)
    .sort((a, b) => b.count - a.count);

  const top = sortedByFrequency.slice(0, limit);
  const others = sortedByFrequency.slice(limit);

  const result = [...top];

  // Always add "others" entry, even if empty
  // Use 0 as delta for "others" (will be displayed as "others" in the chart)
  const othersCount = others.length > 0 ? others.reduce((acc, item) => acc + item.count, 0) : 0;
  result.push({
    delta: 0,
    count: othersCount
  });

  // Sort by frequency descending to match Angular behavior
  return result.sort((a, b) => b.count - a.count);
}

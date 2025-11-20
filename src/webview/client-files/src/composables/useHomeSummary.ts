/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/composables/useHomeSummary.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
/**
 * Composable for Home Summary Computed Properties
 * Provides formatted values and utilities for home summary data
 */

import { computed, type Ref } from 'vue';
import { humanReadable } from '@/lib/helpers';
import { formatExecTime, formatCpuFreq, summaryViewEntries } from '@/utils/home/formatters';
import type { HomeSummaryData, SummaryViewEntry } from '@/types/home';

export function useHomeSummary(data: Ref<HomeSummaryData | undefined>) {
  /**
   * Formatted execution time (HH:MM:SS)
   */
  const formattedExecTime = computed(() => {
    if (!data.value) return '00:00:00';
    return formatExecTime(data.value.run.runtime, data.value.system.ticksPerSecond);
  });

  /**
   * Formatted CPU frequency
   */
  const formattedCpuFreq = computed(() => {
    if (!data.value) return '0 Hz';
    return formatCpuFreq(data.value.system.ticksPerSecond);
  });

  /**
   * Formatted total memory
   */
  const formattedTotalMemory = computed(() => {
    if (!data.value) return '0 B';
    return humanReadable(data.value.system.totalMemory, 1, 'B', true);
  });

  /**
   * Allocator wrappers as comma-separated string
   */
  const allocatorWrappers = computed(() => {
    if (!data.value) return '';
    return data.value.run.allocatorWrappers.join(', ');
  });

  /**
   * Allocator name
   */
  const allocator = computed(() => {
    if (!data.value) return '';
    return data.value.run.allocator || '';
  });

  /**
   * Check if a summary key has warnings
   */
  const hasSummaryWarnings = (key: string): boolean => {
    if (!data.value) return false;
    const warnings = data.value.summaryWarnings[key];
    return warnings !== undefined && warnings.length > 0;
  };

  /**
   * Get formatted value for a summary entry
   */
  const getFormattedValue = (entry: SummaryViewEntry): string => {
    if (!data.value || !entry.key || entry.type === 'separator' || !entry.format) {
      return '';
    }
    const value = data.value.summary[entry.key];
    if (value === undefined) return '';
    return entry.format(value) ?? '';
  };

  /**
   * Get formatted value from key
   */
  const getFormattedValueFromKey = (key: string): string => {
    if (!data.value) return '';
    const entry = summaryViewEntries.find((e) => e.key === key);
    if (!entry) return '';
    return getFormattedValue(entry) ?? '';
  };

  /**
   * Format count (human readable)
   */
  const getHumanCount = (count: number): string => {
    return humanReadable(count, 1, '', true);
  };

  /**
   * Format memory (human readable)
   */
  const getHumanMem = (bytes: number): string => {
    return humanReadable(bytes, 1, 'B', true);
  };

  /**
   * Check if domains data exists
   */
  const hasDomains = computed(() => {
    return data.value?.summaryDomains !== undefined;
  });

  return {
    // Formatted values
    formattedExecTime,
    formattedCpuFreq,
    formattedTotalMemory,
    allocatorWrappers,
    allocator,

    // Utilities
    hasSummaryWarnings,
    getFormattedValue,
    getFormattedValueFromKey,
    getHumanCount,
    getHumanMem,
    hasDomains,

    // Config
    summaryViewEntries,
  };
}

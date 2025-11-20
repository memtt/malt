/**
 * Composable for Realloc Summary
 * Responsible ONLY for calculating summary metrics
 */

import { computed, type Ref } from 'vue';
import { humanReadable } from '@/lib/helpers';
import type { ReallocData } from '@/types/realloc';
import { findMostUsed, calculateMeanDelta, calculateTotalCount } from '@/utils/realloc/calculators';

export function useReallocSummary(data: Ref<ReallocData>) {
  const mostUsed = computed(() => findMostUsed(data.value));

  const meanDelta = computed(() => calculateMeanDelta(data.value));

  const totalCount = computed(() => calculateTotalCount(data.value));

  const meanDeltaFormatted = computed(() =>
    humanReadable(meanDelta.value, 1, 'B', true)
  );

  return {
    mostUsed,
    meanDelta,
    totalCount,
    meanDeltaFormatted,
  };
}

/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/composables/useGlobalVarsSummary.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
/**
 * Composable for Global Variables Summary
 * Responsible ONLY for calculating summary metrics
 */

import { computed, type Ref } from 'vue';
import { humanReadable } from '@/lib/helpers';
import { calculateSummary } from '@/utils/globalVars/calculators';
import type { GlobalVarsData } from '@/types/global-vars';

export function useGlobalVarsSummary(data: Ref<GlobalVarsData | undefined>) {
  const maxThreadCount = computed(() => data.value?.maxThreadCount ?? 1);

  const summary = computed(() =>
    calculateSummary(data.value ?? null, maxThreadCount.value)
  );

  const globalVariablesMem = computed(() =>
    humanReadable(summary.value.global, 1, 'B', true)
  );

  const tlsVariablesMem = computed(() =>
    humanReadable(summary.value.tls, 1, 'B', true)
  );

  const activeThreads = computed(() => maxThreadCount.value);

  return {
    maxThreadCount,
    summary,
    globalVariablesMem,
    tlsVariablesMem,
    activeThreads,
  };
}

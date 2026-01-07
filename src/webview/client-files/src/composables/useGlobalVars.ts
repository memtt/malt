/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/composables/useGlobalVars.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
/**
 * Composable for Global Variables - Composition Layer
 * Combines data, summary, and charts composables
 */

import { useGlobalVarsData } from '@/composables/useGlobalVarsData'
import { useGlobalVarsSummary } from '@/composables/useGlobalVarsSummary'
import { useGlobalVarsCharts } from '@/composables/useGlobalVarsCharts'
import { useGlobalVarsTopList } from '@/composables/useGlobalVarsTopList'

export function useGlobalVars() {
  // Get data
  const { data, loading, error } = useGlobalVarsData();

  // Get summary
  const { maxThreadCount, globalVariablesMem, tlsVariablesMem, activeThreads } =
    useGlobalVarsSummary(data);

  // Get charts
  const {
    dataForBinaryTreemap,
    dataForVariableTreemap,
    dataForBinaryBar,
    dataForVarBar,
    onVarChartValueClick,
    initializeVarBarData,
  } = useGlobalVarsCharts(data, maxThreadCount);

  // Get top list
  const { searchQuery, filteredVariables } = useGlobalVarsTopList(data, maxThreadCount);

  return {
    // Data state
    loading,
    error,

    // Summary
    globalVariablesMem,
    tlsVariablesMem,
    activeThreads,

    // Chart data
    dataForBinaryTreemap,
    dataForVariableTreemap,
    dataForBinaryBar,
    dataForVarBar,
    onVarChartValueClick,
    initializeVarBarData,

    // Top list
    topListSearchQuery: searchQuery,
    topListVariables: filteredVariables,
  };
}

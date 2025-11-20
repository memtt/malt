/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/composables/useGlobalVarsCharts.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
/**
 * Composable for Global Variables Charts
 * Responsible ONLY for formatting chart data
 */

import { ref, computed, watch, type Ref } from 'vue';
import {
  transformToBinaryTreemap,
  transformToVariableTreemap,
  transformToBinaryBarData,
  transformToVarBarData,
} from '@/utils/globalVars/transformers';
import { cutDataForBarChart, formatDataForBarChart } from '@/utils/globalVars/formatters';
import type { GlobalVarsData, BarSeries } from '@/types/global-vars';

export function useGlobalVarsCharts(
  data: Ref<GlobalVarsData | undefined>,
  maxThreadCount: Ref<number>
) {
  const dataForVarBar = ref<BarSeries[]>([]);

  // Computed data for treemap charts
  const dataForBinaryTreemap = computed(() => {
    return transformToBinaryTreemap(data.value ?? null, maxThreadCount.value);
  });

  const dataForVariableTreemap = computed(() => {
    return transformToVariableTreemap(data.value ?? null, maxThreadCount.value);
  });

  // Computed data for binary bar chart
  const dataForBinaryBar = computed(() => {
    const rawData = transformToBinaryBarData(data.value ?? null, maxThreadCount.value);
    return formatDataForBarChart(rawData, maxThreadCount.value);
  });

  // Initialize variable bar chart data
  const initializeVarBarData = () => {
    if (data.value) {
      const rawData = transformToVarBarData(data.value, maxThreadCount.value);
      const cutData = cutDataForBarChart(rawData);
      dataForVarBar.value = formatDataForBarChart(cutData, maxThreadCount.value);
    }
  };

  // Watch for data changes to reinitialize
  watch(
    () => data.value,
    (newData) => {
      if (newData) {
        initializeVarBarData();
      }
    }
  );

  // Handle chart value click (filter variables by binary)
  const onVarChartValueClick = (_event: MouseEvent, name: string) => {
    const rawData = transformToVarBarData(
      data.value ?? null,
      maxThreadCount.value,
      (x) => x === name
    );
    const cutData = cutDataForBarChart(rawData);
    dataForVarBar.value = formatDataForBarChart(cutData, maxThreadCount.value);
  };

  return {
    dataForBinaryTreemap,
    dataForVariableTreemap,
    dataForBinaryBar,
    dataForVarBar,
    onVarChartValueClick,
    initializeVarBarData,
  };
}

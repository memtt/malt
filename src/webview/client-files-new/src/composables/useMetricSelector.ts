/**
 * Composable for Metric Selector State Management
 * Manages metric selection, filtering, sorting, pagination
 * Ported from Angular MaltSelector class
 */

import { ref, computed, watch, type Ref } from 'vue';
import { FUNC_METRICS } from '@/utils/metrics';
import type {
  MetricKey,
  MetricOrder,
  FunctionStat,
  StackInfoData,
} from '@/types/sources';

export interface MetricSelectorOptions {
  initialMetric?: MetricKey;
  initialInclusive?: boolean;
  initialPerPage?: number;
}

export function useMetricSelector(
  functions: Readonly<Ref<FunctionStat[]>>,
  options: MetricSelectorOptions = {},
) {
  // State
  const metric = ref<MetricKey>(options.initialMetric || 'alloc.count');
  const inclusive = ref(options.initialInclusive ?? true);
  const ratio = ref(false);
  const query = ref('');
  const currentPage = ref(1);
  const perPage = ref(options.initialPerPage || 10);

  // Computed order based on current metric
  const order = ref<MetricOrder>(FUNC_METRICS[metric.value].defaultOrder);

  /**
   * Get value from function stat based on current metric and inclusive mode
   */
  const getValue = (func: FunctionStat): number => {
    const metricDef = FUNC_METRICS[metric.value];

    if (!func) return 0;

    let data: StackInfoData | undefined;

    if (inclusive.value) {
      data = func.total;
    } else {
      data = func.own;
    }

    if (!data) return 0;

    // Use the extractor with the data (total or own)
    return metricDef.extractor(data);
  };

  /**
   * Filter function for display
   */
  const filter = (func: FunctionStat): boolean => {
    const value = getValue(func);
    const matchesQuery =
      query.value === '' || func.function.indexOf(query.value) > -1;
    return value > 0 && matchesQuery;
  };

  /**
   * Filtered functions
   */
  const filteredFunctions = computed(() => {
    return functions.value.filter(filter);
  });

  /**
   * Compute reference value (max or sum) for ratio calculation
   * Uses all functions (not filtered) to maintain consistent reference across pages
   */
  const computeRef = computed(() => {
    const metricDef = FUNC_METRICS[metric.value];
    let result = 0;

    if (metricDef.ref === 'sum') {
      result = functions.value.reduce(
        (sum: number, func: FunctionStat) => sum + getValue(func),
        0,
      );
    } else if (metricDef.ref === 'max') {
      result = Math.max(
        ...functions.value.map((func: FunctionStat) => getValue(func)),
        0, // Default value if array is empty
      );
    }

    return result || 1; // Avoid division by zero
  });

  /**
   * Get value as ratio (percentage)
   */
  const getValueRatio = (func: FunctionStat): number => {
    return (100 * getValue(func)) / computeRef.value;
  };

  /**
   * Get formatted value (with unit or percentage)
   */
  const getFormattedValue = (func: FunctionStat): string => {
    if (ratio.value) {
      return `${getValueRatio(func).toFixed(1)}%`;
    }

    const metricDef = FUNC_METRICS[metric.value];
    const value = getValue(func);
    return metricDef.formatter(value);
  };

  /**
   * Total elements after filtering
   */
  const totalElements = computed(() => filteredFunctions.value.length);

  /**
   * Check if order is reversed (descending)
   */
  const isReversedOrder = computed(() => order.value === 'desc');

  /**
   * Get current metric name for display
   */
  const currentMetricName = computed(
    () => FUNC_METRICS[metric.value].name,
  );

  // Actions

  /**
   * Toggle sort order
   */
  const toggleOrder = () => {
    order.value = order.value === 'asc' ? 'desc' : 'asc';
  };

  /**
   * Toggle ratio display
   */
  const toggleRatio = () => {
    ratio.value = !ratio.value;
  };

  /**
   * Toggle inclusive/exclusive mode
   */
  const toggleInclusive = () => {
    inclusive.value = !inclusive.value;
  };

  /**
   * Select a metric
   */
  const selectMetric = (newMetric: MetricKey) => {
    metric.value = newMetric;
    order.value = FUNC_METRICS[newMetric].defaultOrder;
  };

  /**
   * Reset to first page when filters change
   */
  watch([metric, inclusive, ratio, query], () => {
    currentPage.value = 1;
  });

  return {
    // State
    metric,
    inclusive,
    ratio,
    order,
    query,
    currentPage,
    perPage,
    totalElements,

    // Computed
    currentMetricName,
    isReversedOrder,
    filteredFunctions,

    // Methods
    getValue,
    getValueRatio,
    getFormattedValue,
    filter,
    toggleOrder,
    toggleRatio,
    toggleInclusive,
    selectMetric,
  };
}

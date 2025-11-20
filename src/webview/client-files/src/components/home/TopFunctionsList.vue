<template>
  <div class="top-functions-list">
    <table v-if="sortedFunctions.length > 0" class="functions-table">
      <thead>
        <tr>
          <th class="col-ratio">Ratio</th>
          <th class="col-value">Value</th>
          <th class="col-function">Function</th>
        </tr>
      </thead>
      <tbody>
        <tr
          v-for="func in visibleFunctions"
          :key="func.function"
          :class="{ 'hidden-row': shouldHide(func) }"
        >
          <td class="col-ratio">
            <div class="progress-bar-container">
              <div class="progress-bar" :style="{ width: `${getValueRatio(func)}%` }">
                <span class="sr-only">{{ getValueRatio(func).toFixed(0) }}%</span>
              </div>
            </div>
          </td>
          <td class="col-value">{{ getFormattedValue(func) }}</td>
          <td class="col-function">
            <router-link :to="getFunctionLink(func)" class="function-link">
              {{ func.function }}
            </router-link>
          </td>
        </tr>
      </tbody>
    </table>

    <div v-else class="no-data">No functions available</div>
  </div>
</template>

<script setup lang="ts">
import { computed } from 'vue';
import type { FunctionStat, MetricKey } from '@/types/sources';

interface Props {
  functions: FunctionStat[];
  metric: MetricKey;
  limit?: number;
  inclusive?: boolean;
}

const props = withDefaults(defineProps<Props>(), {
  limit: 5,
  inclusive: false,
});

/**
 * Get metric value for a function
 */
const getValue = (func: FunctionStat): number => {
  // Use total.alloc.count as the default metric
  return func.total?.alloc?.count || 0;
};

/**
 * Sort functions by value (descending)
 */
const sortedFunctions = computed(() => {
  return [...props.functions].sort((a, b) => getValue(b) - getValue(a));
});

/**
 * Limit to top N functions
 */
const visibleFunctions = computed(() => {
  return sortedFunctions.value.slice(0, props.limit);
});

/**
 * Compute reference value (max value)
 */
const referenceValue = computed(() => {
  if (sortedFunctions.value.length === 0) return 1;
  return getValue(sortedFunctions.value[0]) || 1;
});

/**
 * Get value ratio (percentage)
 */
const getValueRatio = (func: FunctionStat): number => {
  const value = getValue(func);
  return (100 * value) / referenceValue.value;
};

/**
 * Format value for display
 */
const getFormattedValue = (func: FunctionStat): string => {
  const value = getValue(func);

  // Format based on metric type
  if (props.metric.includes('count')) {
    return value.toLocaleString();
  }

  // Format as bytes for memory metrics
  if (value === 0) return '0 B';

  const units = ['B', 'KB', 'MB', 'GB', 'TB'];
  const k = 1024;
  const i = Math.floor(Math.log(value) / Math.log(k));
  const formattedValue = (value / Math.pow(k, i)).toFixed(1);

  return `${formattedValue} ${units[i]}`;
};

/**
 * Check if function should be hidden (value = 0)
 */
const shouldHide = (func: FunctionStat): boolean => {
  return getValue(func) === 0;
};

/**
 * Get router link to sources page
 */
const getFunctionLink = (func: FunctionStat) => {
  return {
    path: '/sources',
    query: {
      func: func.function,
      metric: props.metric,
    },
  };
};
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.top-functions-list {
  width: 100%;
}

.functions-table {
  width: 100%;
  border-collapse: collapse;
  border: 1px solid $gray-200;
}

.functions-table thead {
  background: $gray-50;
}

.functions-table th {
  padding: 0.75rem;
  text-align: left;
  font-weight: $font-weight-semibold;
  font-size: $font-size-sm;
  color: $gray-600;
  border-bottom: 2px solid $gray-200;
}

.functions-table tbody tr {
  border-bottom: 1px solid $gray-200;
  transition: background-color $transition-fast ease;
}

.functions-table tbody tr:hover {
  background: $gray-50;
}

.functions-table tbody tr.hidden-row {
  display: none;
}

.functions-table td {
  padding: 0.75rem;
  font-size: $font-size-sm;
}

.col-ratio {
  width: 30%;
}

.col-value {
  width: 20%;
  text-align: right;
  font-family: $font-family-mono;
  color: $gray-600;
}

.col-function {
  width: 50%;
}

/* Progress bar */
.progress-bar-container {
  width: 100%;
  height: 20px;
  background: $gray-200;
  border-radius: 3px;
  overflow: hidden;
}

.progress-bar {
  height: 100%;
  background: linear-gradient(90deg, #007bff 0%, #0056b3 100%);
  transition: width $transition-slow ease;
  display: flex;
  align-items: center;
  justify-content: center;
  color: white;
  font-size: $font-size-xs;
  font-weight: $font-weight-semibold;
}

.sr-only {
  position: absolute;
  width: 1px;
  height: 1px;
  padding: 0;
  margin: -1px;
  overflow: hidden;
  clip: rect(0, 0, 0, 0);
  white-space: nowrap;
  border: 0;
}

/* Function link */
.function-link {
  color: #007bff;
  text-decoration: none;
  font-family: $font-family-mono;
  transition: color $transition-fast ease;
}

.function-link:hover {
  color: #0056b3;
  text-decoration: underline;
}

/* No data */
.no-data {
  padding: 2rem;
  text-align: center;
  color: $color-text-muted;
  font-style: italic;
}
</style>

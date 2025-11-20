<template>
  <div class="sources-sidebar">
    <!-- Metric Selector -->
    <MetricSelector
      :current-metric-name="selector.currentMetricName.value"
      :is-ascending="!selector.isReversedOrder.value"
      :ratio="selector.ratio.value"
      :inclusive="selector.inclusive.value"
      :query="selector.query.value"
      @toggle-order="selector.toggleOrder"
      @toggle-ratio="selector.toggleRatio"
      @toggle-inclusive="selector.toggleInclusive"
      @select-metric="selector.selectMetric"
      @update:query="selector.query.value = $event"
    />

    <!-- Function List -->
    <FunctionList
      :functions="selector.filteredFunctions.value"
      :all-functions="allFunctions"
      :get-formatted-value="selector.getFormattedValue"
      :get-value-ratio="selector.getValueRatio"
      :get-value="selector.getValue"
      :is-reversed-order="selector.isReversedOrder.value"
      :current-page="selector.currentPage.value"
      :per-page="selector.perPage.value"
      :selected-function="selectedFunction"
      :compact="true"
      @select="emit('functionSelect', $event)"
      @update:current-page="selector.currentPage.value = $event"
    />
  </div>
</template>

<script setup lang="ts">
import MetricSelector from '@/components/core/MetricSelector.vue'
import FunctionList from '@/components/shared/FunctionList.vue'
import type { FunctionStat } from '@/types/sources'

interface Props {
  selector: ReturnType<typeof import('@/composables/useMetricSelector').useMetricSelector>
  allFunctions: FunctionStat[]
  selectedFunction?: string
}

interface Emits {
  (e: 'functionSelect', func: FunctionStat): void
}

const { selector, allFunctions, selectedFunction } = defineProps<Props>()
const emit = defineEmits<Emits>()
</script>

<style scoped>
.sources-sidebar {
  display: flex;
  flex-direction: column;
  height: 100%;
}
</style>

<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/call-tree/CallTreeFilters.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->

<template>
  <div class="call-tree-filters">
    <div class="filters-header">
      <h3 class="filters-title">Filter Graph Nodes</h3>
      <button
        v-if="visibleNodes !== undefined && totalNodes !== undefined"
        class="btn-fit"
        @click="emit('fit-graph')"
        title="Resize to fit"
      >
        ⛶ Fit
      </button>
    </div>

    <!-- Node statistics -->
    <div v-if="visibleNodes !== undefined && totalNodes !== undefined" class="node-stats">
      <span v-if="visibleNodes === totalNodes" class="stats-text">
        Showing all nodes
      </span>
      <span v-else class="stats-text">
        Showing <strong>{{ visibleNodes }}</strong> out of <strong>{{ totalNodes }}</strong> nodes
      </span>
    </div>

    <!-- Filter controls -->
    <div class="filter-controls">
      <!-- Height Filter -->
      <div class="filter-group">
        <label for="filter-height" class="filter-label">Height</label>
        <select
          id="filter-height"
          v-model.number="localHeight"
          class="filter-select"
          @change="emitUpdate"
        >
          <option v-for="option in heightOptions" :key="option" :value="option">
            {{ getFilterLabel(option) }}
          </option>
        </select>
      </div>

      <!-- Depth Filter -->
      <div class="filter-group">
        <label for="filter-depth" class="filter-label">Depth</label>
        <select
          id="filter-depth"
          v-model.number="localDepth"
          class="filter-select"
          @change="emitUpdate"
        >
          <option v-for="option in depthOptions" :key="option" :value="option">
            {{ getFilterLabel(option) }}
          </option>
        </select>
      </div>

      <!-- Node Cost Filter -->
      <div class="filter-group">
        <label for="filter-node-cost" class="filter-label">Node Cost</label>
        <select
          id="filter-node-cost"
          v-model.number="localNodeCost"
          class="filter-select"
          @change="emitUpdate"
        >
          <option v-for="option in nodeCostOptions" :key="option" :value="option">
            {{ getNodeCostLabel(option) }}
          </option>
        </select>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, watch } from 'vue';
import {
  HEIGHT_OPTIONS,
  DEPTH_OPTIONS,
  NODE_COST_OPTIONS,
  getFilterLabel,
  getNodeCostLabel,
} from '@/composables/useCallTreeFilters';

interface Props {
  height: number;
  depth: number;
  nodeCost: number;
  visibleNodes?: number;
  totalNodes?: number;
}

interface Emits {
  (e: 'update:height', value: number): void;
  (e: 'update:depth', value: number): void;
  (e: 'update:nodeCost', value: number): void;
  (e: 'fit-graph'): void;
}

const props = defineProps<Props>();
const emit = defineEmits<Emits>();

// Local state for v-model
const localHeight = ref(props.height);
const localDepth = ref(props.depth);
const localNodeCost = ref(props.nodeCost);

// Filter options
const heightOptions = HEIGHT_OPTIONS;
const depthOptions = DEPTH_OPTIONS;
const nodeCostOptions = NODE_COST_OPTIONS;

/**
 * Emit updates to parent
 */
const emitUpdate = () => {
  emit('update:height', localHeight.value);
  emit('update:depth', localDepth.value);
  emit('update:nodeCost', localNodeCost.value);
};

/**
 * Watch for external prop changes
 */
watch(
  () => props.height,
  (newValue) => {
    localHeight.value = newValue;
  },
);

watch(
  () => props.depth,
  (newValue) => {
    localDepth.value = newValue;
  },
);

watch(
  () => props.nodeCost,
  (newValue) => {
    localNodeCost.value = newValue;
  },
);
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.call-tree-filters {
  background: $color-bg-primary;
  border: 1px solid $gray-200;
  border-radius: $spacing-xs;
  padding: 1rem;
}

/* Header */
.filters-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 0.75rem;
}

.filters-title {
  font-size: 1rem;
  font-weight: $font-weight-semibold;
  color: $gray-900;
  margin: 0;
}

.btn-fit {
  background: #007bff;
  color: white;
  border: none;
  border-radius: $spacing-xs;
  padding: 0.375rem 0.75rem;
  font-size: $font-size-sm;
  cursor: pointer;
  transition: background-color $transition-fast ease;
  display: flex;
  align-items: center;
  gap: 0.25rem;
}

.btn-fit:hover {
  background: #0056b3;
}

.btn-fit:active {
  transform: scale(0.98);
}

/* Node statistics */
.node-stats {
  background: $gray-50;
  border: 1px solid $gray-200;
  border-radius: $spacing-xs;
  padding: 0.5rem 0.75rem;
  margin-bottom: 1rem;
  text-align: center;
}

.stats-text {
  font-size: $font-size-sm;
  color: $gray-600;
}

.stats-text strong {
  color: $gray-900;
  font-weight: $font-weight-semibold;
}

/* Filter controls */
.filter-controls {
  display: flex;
  flex-direction: column;
  gap: 0.75rem;
}

.filter-group {
  display: flex;
  align-items: center;
  gap: 0.75rem;
}

.filter-label {
  font-size: $font-size-sm;
  font-weight: $font-weight-medium;
  color: $gray-600;
  min-width: 80px;
  margin: 0;
}

.filter-select {
  flex: 1;
  padding: 0.375rem 0.75rem;
  font-size: $font-size-sm;
  border: 1px solid #ced4da;
  border-radius: $spacing-xs;
  background: $color-bg-primary;
  color: $gray-600;
  cursor: pointer;
  transition: border-color $transition-fast ease;
}

.filter-select:hover {
  border-color: #adb5bd;
}

.filter-select:focus {
  outline: none;
  border-color: #007bff;
  box-shadow: 0 0 0 0.2rem rgba(0, 123, 255, 0.25);
}

/* Responsive */
@media (min-width: 768px) {
  .filter-controls {
    flex-direction: row;
    flex-wrap: wrap;
  }

  .filter-group {
    flex: 1;
    min-width: 150px;
  }
}
</style>

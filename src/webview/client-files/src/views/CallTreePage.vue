<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/views/CallTreePage.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
|    AUTHOR   : Sébastien Valat (INRIA) - 2025
----------------------------------------------------------->
<template>
  <div class="call-tree-page">
    <!-- Loading State -->
    <div v-if="sourcesLoading" class="loading-state">
      <div class="spinner-border" role="status">
        <span class="visually-hidden">Loading...</span>
      </div>
      <p class="loading-text">Loading call tree data...</p>
    </div>

    <!-- Error State -->
    <div v-else-if="sourcesError" class="alert alert-danger" role="alert">
      <h4 class="alert-heading">Error loading data</h4>
      <p>{{ sourcesError }}</p>
    </div>

    <!-- Main Content -->
    <div v-else class="call-tree-layout">
      <!-- Sidebar -->
      <aside class="call-tree-sidebar-container">
        <SourcesSidebar
          :selector="selector"
          :all-functions="functions"
          :selected-function="selectedFunction"
          @function-select="onFunctionSelect"
        />
      </aside>

      <!-- Main Content Area -->
      <main class="call-tree-main">
        <!-- Navigation Header with Controls -->
        <div class="navigation-header">
          <!-- Filter Controls (Height, Depth, Node Cost) -->
          <div class="filter-controls-inline">
            <div class="filter-group-inline">
              <label for="filter-height" class="filter-label-inline">Height:</label>
              <select
                id="filter-height"
                v-model.number="filters.height.value"
                class="filter-select-inline"
              >
                <option v-for="option in heightOptions" :key="option" :value="option">
                  {{ getFilterLabel(option) }}
                </option>
              </select>
            </div>

            <div class="filter-group-inline">
              <label for="filter-depth" class="filter-label-inline">Depth:</label>
              <select
                id="filter-depth"
                v-model.number="filters.depth.value"
                class="filter-select-inline"
              >
                <option v-for="option in depthOptions" :key="option" :value="option">
                  {{ getFilterLabel(option) }}
                </option>
              </select>
            </div>

            <div class="filter-group-inline">
              <label for="filter-node-cost" class="filter-label-inline">Node Cost:</label>
              <select
                id="filter-node-cost"
                v-model.number="filters.nodeCost.value"
                class="filter-select-inline"
              >
                <option v-for="option in nodeCostOptions" :key="option" :value="option">
                  {{ getNodeCostLabel(option) }}
                </option>
              </select>
            </div>
          </div>

          <!-- Back & Forward buttons -->
          <!-- <div class="navigation-buttons">
            <button
              class="nav-button"
              :disabled="!canGoBack"
              @click="onNavigateBack"
              title="Go back"
            >
              <span class="nav-icon">‹</span>
              Back
            </button>
            <button
              class="nav-button"
              :disabled="!canGoForward"
              @click="onNavigateForward"
              title="Go forward"
            >
              Forward
              <span class="nav-icon">›</span>
            </button>
          </div> -->
        </div>

        <!-- File Header -->
        <FileHeader
          v-if="callTreeData"
          :file-path="callTreeData.file"
          :function-name="callTreeData.function"
        />

        <!-- SVG Viewer -->
        <div class="svg-viewer-container">
          <CallTreeSvgViewer
            ref="svgViewerRef"
            :data="callTreeData"
            :loading="callTreeLoading"
            :error="callTreeError"
            :metric="selectedMetric"
            :ratio="ratio"
            :depth="filters.depth.value"
            :height="filters.height.value"
            :node-cost="filters.nodeCost.value"
            :current-node-id="callTreeData?.nodeId ?? -1"
            @node-select="onNodeSelect"
          />
        </div>

        <!-- Bottom Section: Memory Table (Full Width) -->
        <!-- <div class="bottom-section">
          <MemoryStatsTable :details="selectedDetails" />
        </div> -->
      </main>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref } from 'vue'
import { useSources } from '@/composables/useSources'
import { useCallTree } from '@/composables/useCallTree'
import {
  HEIGHT_OPTIONS,
  DEPTH_OPTIONS,
  NODE_COST_OPTIONS,
  getFilterLabel,
  getNodeCostLabel,
} from '@/composables/useCallTreeFilters'
import SourcesSidebar from '@/components/sources/SourcesSidebar.vue'
import CallTreeSvgViewer from '@/components/call-tree/CallTreeSvgViewer.vue'
import FileHeader from '@/components/shared/FileHeader.vue'
import type { FunctionStat, SourceAnnotation } from '@/types/sources'

// Filter options
const heightOptions = HEIGHT_OPTIONS
const depthOptions = DEPTH_OPTIONS
const nodeCostOptions = NODE_COST_OPTIONS

// Refs
const svgViewerRef = ref<InstanceType<typeof CallTreeSvgViewer> | null>(null)
const selectedDetails = ref<SourceAnnotation | null>(null)
const selectedFunction = ref<string | undefined>(undefined)
const previousFunction = ref<string | null>(null)

// Get initial metric from route query
const initialMetric = 'peakmem.global'

// Sources data (for function list in sidebar)
const {
  loading: sourcesLoading,
  error: sourcesError,
  functions,
  selector,
} = useSources({
  initialMetric: initialMetric as import('@/types/sources').MetricKey,
})

// Call tree data
const {
  data: callTreeData,
  loading: callTreeLoading,
  error: callTreeError,
  filters,
  // canGoBack,
  // canGoForward,
  selectedMetric,
  ratio,
  loadByNodeId,
  loadByFunction,
  goBack,
  goForward,
} = useCallTree({
  metric: selector.metric,
  ratio: selector.ratio,
})

/**
 * Handle function selection from sidebar
 */
const onFunctionSelect = async (func: FunctionStat) => {
  // Skip if same function clicked (deduplication)
  if (previousFunction.value === func.function) {
    return
  }
  previousFunction.value = func.function

  selectedFunction.value = func.function
  selectedDetails.value = null

  // Load call tree for this function
  await loadByFunction(func)

  // Set selected details to function stats
  selectedDetails.value = {
    line: func.line || 0,
    file: func.file,
    function: func.function,
    total: func.total,
    own: func.own,
  }
}

/**
 * Handle node selection from SVG viewer
 */
const onNodeSelect = async (nodeId: number) => {
  await loadByNodeId(nodeId, true)

  // Update selected details if we have data
  if (callTreeData.value) {
    const func = functions.value.find((f) => f.function === callTreeData.value?.function)
    if (func) {
      selectedDetails.value = {
        line: func.line || 0,
        file: func.file,
        function: func.function,
        total: func.total,
        own: func.own,
      }
      selectedFunction.value = func.function
    }
  }
}

/**
 * Navigate back in history
 */
const onNavigateBack = async () => {
  await goBack()

  // Update selected details
  if (callTreeData.value) {
    const func = functions.value.find((f) => f.function === callTreeData.value?.function)
    if (func) {
      selectedDetails.value = {
        line: func.line || 0,
        file: func.file,
        function: func.function,
        total: func.total,
        own: func.own,
      }
      selectedFunction.value = func.function
    }
  }
}

/**
 * Navigate forward in history
 */
const onNavigateForward = async () => {
  await goForward()

  // Update selected details
  if (callTreeData.value) {
    const func = functions.value.find((f) => f.function === callTreeData.value?.function)
    if (func) {
      selectedDetails.value = {
        line: func.line || 0,
        file: func.file,
        function: func.function,
        total: func.total,
        own: func.own,
      }
      selectedFunction.value = func.function
    }
  }
}
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.call-tree-page {
  display: flex;
  flex-direction: column;
  border: 1px solid $color-border-light;
  border-radius: $radius-xl;
  background: $blue-section-bg;
}

.loading-state {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  height: 100vh;
  background-color: $blue-section-bg;
  border-radius: $radius-xl;
}

.call-tree-layout {
  display: flex;
  flex: 1;
  overflow: hidden;
  padding: $spacing-3xl;
  gap: $spacing-3xl;
}

/* Sidebar */
.call-tree-sidebar-container {
  width: 320px;
  flex-shrink: 0;
  overflow: hidden;
}

/* Main Content */
.call-tree-main {
  flex: 1;
  display: flex;
  flex-direction: column;
  overflow: hidden;
}

/* Navigation Header */
.navigation-header {
  display: flex;
  align-items: center;
  justify-content: flex-start;
  background-color: $blue-section-bg;
  gap: $spacing-xl;
  margin-bottom: $spacing-lg;
}

.navigation-buttons {
  display: flex;
  gap: $spacing-xs;
}

.nav-button {
  display: flex;
  align-items: center;
  gap: 0.25rem;
  padding: 0.375rem 0.75rem;
  background: white;
  border: 1px solid #ced4da;
  border-radius: $spacing-xs;
  font-size: $font-size-sm;
  color: #495057;
  cursor: pointer;
  transition: all $transition-fast ease;
}

.nav-button:hover:not(:disabled) {
  background: $gray-50;
  border-color: #adb5bd;
}

.nav-button:disabled {
  opacity: 0.5;
  cursor: not-allowed;
}

.nav-icon {
  font-size: 1.25rem;
  font-weight: bold;
  line-height: 1;
}

/* Filter Controls Inline */
.filter-controls-inline {
  display: flex;
  align-items: center;
  gap: $spacing-xl;
  flex: 1;
}

.filter-group-inline {
  display: flex;
  align-items: center;
  gap: $spacing-xs;
}

.filter-label-inline {
  font-size: $font-size-base;
  font-weight: $font-weight-normal;
  color: $color-text-primary;
  margin: 0;
  white-space: nowrap;
}

.filter-select-inline {
  min-width: 100px;
  padding: 6px $spacing-xl 6px $spacing-md;
  font-size: $font-size-sm;
  font-weight: $font-weight-medium;
  border: none;
  border-radius: $radius-md;
  background: #34495e;
  color: white;
  cursor: pointer;
  appearance: none;
  background-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' width='12' height='12' viewBox='0 0 12 12'%3E%3Cpath fill='white' d='M6 9L1 4h10z'/%3E%3C/svg%3E");
  background-repeat: no-repeat;
  background-position: right 10px center;
  transition: background-color $transition-fast ease;
}

.filter-select-inline:hover {
  background-color: #2c3e50;
}

.filter-select-inline:focus {
  outline: none;
  box-shadow: 0 0 0 3px rgba(52, 73, 94, 0.3);
}

/* SVG Viewer */
.svg-viewer-container {
  flex: 1;
  overflow: hidden;
  background: $gray-50;
  margin-top: $spacing-md;
  border: 1px solid $color-border-light;
  border-radius: $radius-lg;
}

/* Bottom Section */
.bottom-section {
  padding: 1rem;
  background: white;
  border-top: 2px solid #dee2e6;
  overflow-y: auto;
  max-height: 300px;
}

/* Responsive */
@media (max-width: 1200px) {
  .call-tree-sidebar-container {
    width: 300px;
  }

  .bottom-section {
    max-height: none;
  }

  .navigation-header {
    flex-wrap: wrap;
  }

  .filter-controls-inline {
    flex-wrap: wrap;
  }
}

/* Spinner */
.spinner-border {
  width: 3rem;
  height: 3rem;
  border: 0.25em solid $color-primary;
  border-right-color: transparent;
  border-radius: 50%;
  animation: spinner-border 0.75s linear infinite;
}

@keyframes spinner-border {
  to {
    transform: rotate(360deg);
  }
}

.loading-text {
  margin-top: $spacing-md;
  font-family: $font-family-base;
  color: $gray-700;
}

.visually-hidden {
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

/* Alert */
.alert {
  padding: 1rem;
  margin: $spacing-md;
  border: 1px solid transparent;
  border-radius: $spacing-xs;
}

.alert-danger {
  color: #721c24;
  background-color: $color-error-bg;
  border-color: $color-error-border;
}

.alert-heading {
  margin-top: 0;
  margin-bottom: 0.5rem;
}
</style>

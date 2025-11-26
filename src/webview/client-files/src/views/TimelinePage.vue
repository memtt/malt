<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/views/TimelinePage.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->
<template>
  <div class="timeline-page">
    <!-- Loading State -->
    <div v-if="loading" class="loading-container">
      <div class="spinner"></div>
      <p class="loading-text">Loading timeline data...</p>
    </div>

    <!-- Error State -->
    <div v-else-if="error" class="error-container">
      <div class="error-content">
        <h3 class="error-title">Error loading data</h3>
        <p class="error-message">{{ error }}</p>
        <button class="retry-btn" @click="() => refetch()">Retry</button>
      </div>
    </div>

    <!-- Charts -->
    <div v-else class="charts-container">
      <!-- Memory allocated over time -->
      <PageSection title="Memory allocated over time">
        <TimelineChart
          ref="memoryChartRef"
          :data="memoryChartData"
          y-axis-label="Memory"
          y-axis-unit="B"
          chart-type="line"
          :ticks-per-second="ticksPerSecond"
          @dblclick="handleMemoryChartDblClick"
        />
        <ExportControls chart-id="memory" @export="handleExport" />
      </PageSection>

      <!-- Alive chunks over time -->
      <PageSection title="Alive chunks over time">
        <TimelineChart
          ref="chunksChartRef"
          :data="aliveChunksChartData"
          y-axis-label="Alive chunks"
          y-axis-unit=""
          chart-type="line"
          :ticks-per-second="ticksPerSecond"
        />
        <ExportControls chart-id="chunks" @export="handleExport" />
      </PageSection>

      <!-- Allocation rate (memory size) -->
      <PageSection title="Allocation rate (memory size)">
        <TimelineChart
          ref="allocRateSizeChartRef"
          :data="allocRateSizeChartData"
          y-axis-label="Allocation rate"
          y-axis-unit="B/s"
          chart-type="area"
          :ticks-per-second="ticksPerSecond"
        />
        <ExportControls chart-id="alloc-rate-size" @export="handleExport" />
      </PageSection>

      <!-- Allocation rate (operations) -->
      <PageSection title="Allocation rate (operations)">
        <TimelineChart
          ref="allocRateCountChartRef"
          :data="allocRateCountChartData"
          y-axis-label="Allocation rate"
          y-axis-unit="op/s"
          chart-type="area"
          :ticks-per-second="ticksPerSecond"
        />
        <ExportControls chart-id="alloc-rate-count" @export="handleExport" />
      </PageSection>

      <!-- System memory -->
      <PageSection title="System memory">
        <TimelineChart
          ref="systemChartRef"
          :data="systemMemoryChartData"
          y-axis-label="System memory"
          y-axis-unit="B"
          chart-type="line"
          :ticks-per-second="ticksPerSecond"
        />
        <ExportControls chart-id="system" @export="handleExport" />
      </PageSection>
    </div>

    <!-- Allocated Chunks Modal -->
    <AllocatedChunksModal
      :is-open="showChunksModal"
      :chunks="chunksModalData"
      :loading="chunksModalLoading"
      :error="chunksModalError"
      @close="closeChunksModal"
    />
  </div>
</template>

<script setup lang="ts">
import { ref } from 'vue'
import TimelineChart from '@/components/charts/TimelineChart.vue'
import ExportControls from '@/components/core/ExportControls.vue'
import AllocatedChunksModal from '@/components/shared/AllocatedChunksModal.vue'
import PageSection from '@/components/core/PageSection.vue'
import { useTimeline } from '@/composables/useTimeline'

// Use timeline composable
const {
  loading,
  error,
  refetch,
  ticksPerSecond,
  memoryChartData,
  aliveChunksChartData,
  allocRateSizeChartData,
  allocRateCountChartData,
  systemMemoryChartData,
  exportChart,
  fetchActiveChunks,
  showChunksModal,
  chunksModalData,
  chunksModalLoading,
  chunksModalError,
  closeChunksModal,
} = useTimeline()

// Chart refs for SVG export
const memoryChartRef = ref<InstanceType<typeof TimelineChart> | null>(null)
const chunksChartRef = ref<InstanceType<typeof TimelineChart> | null>(null)
const allocRateSizeChartRef = ref<InstanceType<typeof TimelineChart> | null>(null)
const allocRateCountChartRef = ref<InstanceType<typeof TimelineChart> | null>(null)
const systemChartRef = ref<InstanceType<typeof TimelineChart> | null>(null)

/**
 * Get chart ref by chart ID
 */
const getChartRef = (chartId: string) => {
  switch (chartId) {
    case 'memory':
      return memoryChartRef.value
    case 'chunks':
      return chunksChartRef.value
    case 'alloc-rate-size':
      return allocRateSizeChartRef.value
    case 'alloc-rate-count':
      return allocRateCountChartRef.value
    case 'system':
      return systemChartRef.value
    default:
      return null
  }
}

/**
 * Handle export button click
 */
const handleExport = (chartId: string, format: 'json' | 'csv' | 'svg') => {
  const chartRef = getChartRef(chartId)
  const svgElement = chartRef?.svgElement ?? undefined

  exportChart(chartId, format, svgElement)
}

/**
 * Handle double-click on memory chart to show allocated chunks
 */
const handleMemoryChartDblClick = (timestamp: number) => {
  fetchActiveChunks(timestamp)
}
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.timeline-page {
  margin: 0 auto;
}

.page-header {
  margin-bottom: $spacing-2xl;
}

.page-title {
  font-size: $font-size-3xl;
  font-weight: $font-weight-bold;
  color: $gray-900;
  margin-bottom: $spacing-sm;
}

.page-description {
  font-size: $font-size-md;
  color: $color-text-muted;
}

/* Loading State */
.loading-container {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  padding: $spacing-5xl 20px;
  background-color: $blue-section-bg;
  border-radius: $radius-xl;
}

.spinner {
  border: 4px solid $gray-100;
  border-top: 4px solid #2563eb;
  border-radius: 50%;
  width: $spacing-4xl;
  height: $spacing-4xl;
  animation: spin 1s linear infinite;
  margin-bottom: $spacing-lg;
}

@keyframes spin {
  0% {
    transform: rotate(0deg);
  }
  100% {
    transform: rotate(360deg);
  }
}

.loading-text {
  font-size: $font-size-md;
  color: $color-text-muted;
}

/* Error State */
.error-container {
  display: flex;
  justify-content: center;
  padding: $spacing-5xl 20px;
}

.error-content {
  text-align: center;
  max-width: 400px;
}

.error-title {
  font-size: $font-size-xl;
  font-weight: $font-weight-semibold;
  color: $color-error;
  margin-bottom: $spacing-sm;
}

.error-message {
  font-size: $font-size-sm;
  color: $color-text-muted;
  margin-bottom: $spacing-lg;
}

.retry-btn {
  padding: $spacing-sm 20px;
  background-color: #2563eb;
  color: white;
  border: none;
  border-radius: $radius-sm;
  font-size: $font-size-sm;
  font-weight: $font-weight-medium;
  cursor: pointer;
  transition: background-color $transition-base;
}

.retry-btn:hover {
  background-color: #1d4ed8;
}

/* Export Modal */
.export-data {
  background-color: $gray-50;
  border: 1px solid $gray-200;
  border-radius: $spacing-xs;
  padding: $spacing-lg;
  font-family: $font-family-mono;
  font-size: $font-size-xs;
  overflow-x: auto;
  max-height: 500px;
  white-space: pre;
  margin: 0;
}

.btn {
  padding: $spacing-sm $spacing-lg;
  border-radius: $radius-sm;
  font-size: $font-size-sm;
  font-weight: $font-weight-medium;
  cursor: pointer;
  transition: all $transition-base;
  border: 1px solid transparent;
}

.btn-default {
  background-color: $gray-100;
  color: $gray-700;
  border-color: $gray-300;
}

.btn-default:hover {
  background-color: $gray-200;
}

.btn-primary {
  background-color: $color-primary;
  color: white;
}

.btn-primary:hover {
  background-color: $color-primary-light;
}
</style>

<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/views/StackPeaksPage.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->
<template>
  <div class="stack-peaks-page">
    <!-- Loading State -->
    <div v-if="loading" class="loading-container">
      <div class="spinner"></div>
      <p class="loading-text">Loading stack peaks data...</p>
    </div>

    <!-- Error State -->
    <div v-else-if="error" class="error-container">
      <div class="error-content">
        <h3 class="error-title">Error loading data</h3>
        <p class="error-message">{{ error }}</p>
        <button class="retry-btn" @click="() => refetch()">Retry</button>
      </div>
    </div>

    <!-- Main Content with Sidebar -->
    <div v-else class="stack-peaks-layout">
      <!-- Summary Section -->

      <SummaryCard :items="summaryItems" />

      <!-- Main Content Area -->
      <main class="main-content">
        <!-- Sidebar -->
        <StackPeaksSidebar
          :threads="data?.stacks || []"
          :selected-thread-id="selectedThreadId"
          :largest-stack="largestStack"
          @thread-select="handleThreadSelect"
        />

        <div class="main-charts">
          <!-- Stack Peaks Chart -->
          <PageSection>
            <template #title>Stack<span>peaks</span></template>
            <StackPeaksBarChart
              :data="stackPeaksChartData"
              :selected-index="selectedThreadId"
              @bar-click="handleThreadSelect"
            />
          </PageSection>

          <!-- Stack Timeline Chart -->
          <PageSection>
            <template #title>
              Stack
              <span v-if="selectedThreadId !== null" class="thread-id"
                >of thread {{ selectedThreadId }}</span
              >
              <span v-else class="thread-id-all">(all threads)</span>
            </template>
            <StackTimelineChart
              :data="stackTimelineChartData"
              :ticks-per-second="ticksPerSecond"
              y-axis-label="Stack size"
              y-axis-unit="B"
            />
          </PageSection>

          <!-- Stack Functions Chart -->
          <PageSection v-if="selectedThreadId !== null">
            <template #title>
              Memory
              <span class="thread-id">used by function for thread {{ selectedThreadId }}</span>
            </template>

            <!-- Loading state for details -->
            <div v-if="detailsLoading" class="details-loading">
              <div class="spinner-small"></div>
              <p>Loading function details...</p>
            </div>

            <!-- Error state for details -->
            <div v-else-if="detailsError" class="details-error">
              <p>Error loading function details: {{ detailsError }}</p>
            </div>

            <!-- Functions chart -->
            <StackFunctionsBarChart
              v-else-if="stackDetailsChartData.length > 0"
              :data="stackDetailsChartData"
            />

            <!-- Empty state -->
            <div v-else class="details-empty">
              <p>No function details available for this thread.</p>
            </div>
          </PageSection>
        </div>
      </main>
    </div>
  </div>
</template>

<script setup lang="ts">
import { computed } from 'vue'
import SummaryCard from '@/components/home/SummaryCard.vue'
import StackPeaksBarChart from '@/components/charts/StackPeaksBarChart.vue'
import StackTimelineChart from '@/components/charts/StackTimelineChart.vue'
import StackFunctionsBarChart from '@/components/charts/StackFunctionsBarChart.vue'
import StackPeaksSidebar from '@/components/stack-peaks/StackPeaksSidebar.vue'
import PageSection from '@/components/core/PageSection.vue'
import { useStackPeaks } from '@/composables/useStackPeaks'
import { humanReadable } from '@/lib/helpers'

// Use stack peaks composable
const {
  data,
  loading,
  error,
  refetch,
  largestStack,
  threadsCount,
  selectedThreadId,
  selectThread,
  stackPeaksChartData,
  stackTimelineChartData,
  stackDetailsChartData,
  ticksPerSecond,
  detailsLoading,
  detailsError,
} = useStackPeaks()

/**
 * Format size value
 */
const formatSize = (bytes: number): string => {
  return humanReadable(bytes, 1, 'B', true)
}

/**
 * Summary items for SummaryCard component
 */
const summaryItems = computed(() => [
  {
    title: 'Largest Stack',
    value: formatSize(largestStack.value),
  },
  {
    title: 'Threads',
    value: threadsCount.value.toString(),
  },
])

/**
 * Handle thread selection from sidebar or bar chart click
 */
const handleThreadSelect = (threadId: number | null): void => {
  selectThread(threadId)
}
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.main-content {
  flex: 1;
  display: flex;
  gap: $spacing-md;
  margin-top: $spacing-md;
  width: 100%;
}

.main-charts {
  flex: 1;
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

.spinner-small {
  border: 3px solid $gray-100;
  border-top: 3px solid #2563eb;
  border-radius: 50%;
  width: $spacing-2xl;
  height: $spacing-2xl;
  animation: spin 1s linear infinite;
  margin-bottom: $spacing-md;
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

/* Content */
.section-title {
  font-size: $font-size-xl;
  font-weight: $font-weight-semibold;
  color: $gray-900;
  margin-bottom: $spacing-lg;
}

/* Details States */
.details-loading,
.details-error,
.details-empty {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  padding: $spacing-3xl 20px;
  text-align: center;
  color: $color-text-muted;
  font-size: $font-size-sm;
}

.details-error {
  color: $color-error;
}

.details-empty {
  color: #9ca3af;
}

/* Responsive */
@media (max-width: 1200px) {
  .sidebar-container {
    width: 250px;
  }
}

@media (max-width: 992px) {
  .sidebar-container {
    width: 100%;
    position: relative;
    top: 0;
    max-height: 400px;
  }

  .main-content {
    width: 100%;
  }
}
</style>

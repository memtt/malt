<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/views/PerThreadPage.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->
<template>
  <div class="per-thread-page">
    <!-- Loading State -->
    <div v-if="loading" class="loading-container">
      <div class="spinner"></div>
      <p class="loading-text">Loading per-thread data...</p>
    </div>

    <!-- Error State -->
    <div v-else-if="error" class="error-container">
      <div class="error-content">
        <h3 class="error-title">Error loading data</h3>
        <p class="error-message">{{ error }}</p>
        <button class="retry-btn" @click="() => refetch()">Retry</button>
      </div>
    </div>

    <!-- Main Content -->
    <div v-else class="charts-container">
      <!-- Call per thread -->
      <PageSection title="Call per thread">
        <PerThreadBarChart :data="callCountChartData" metric="count" />
      </PageSection>

      <!-- Time per thread -->
      <PageSection title="Time per thread">
        <PerThreadBarChart :data="timeChartData" metric="time" :ticks-per-second="ticksPerSecond" />
      </PageSection>

      <!-- Cumulated memory per thread -->
      <PageSection title="Cumulated memory per thread">
        <PerThreadBarChart :data="memoryChartData" metric="sum" />
      </PageSection>
    </div>
  </div>
</template>

<script setup lang="ts">
import PerThreadBarChart from '@/components/charts/PerThreadBarChart.vue'
import PageSection from '@/components/core/PageSection.vue'
import { usePerThread } from '@/composables/usePerThread'

// Use per-thread composable
const {
  loading,
  error,
  refetch,
  callCountChartData,
  timeChartData,
  memoryChartData,
  ticksPerSecond,
} = usePerThread()
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.per-thread-page {
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
</style>

<template>
  <div class="alloc-size-distr-page">
    <!-- Summary Cards -->
    <SummaryCard :items="summaryItems" />

    <!-- Most used sizes -->
    <PageSection title="Most used sizes">
      <div v-if="loading" class="chart-loading-state">
        <div class="loading-message">Loading...</div>
      </div>

      <div v-else-if="error" class="chart-error-state">
        <p class="error-message">Error: {{ error }}</p>
      </div>

      <MostUsedSizesChart v-else :data="mostUsedSizesData" :height="400" />
    </PageSection>

    <!-- Histogram -->
    <PageSection title="Histogram">
      <div v-if="loading" class="chart-loading-state">
        <div class="loading-message">Loading...</div>
      </div>

      <div v-else-if="error" class="chart-error-state">
        <p class="error-message">Error: {{ error }}</p>
      </div>

      <Log2HistogramChart v-else :data="histogramData" :height="400" />
    </PageSection>

    <!-- Size over time - Heatmap -->
    <PageSection title="Size over time">
      <div v-if="scatterLoading" class="flex items-center justify-center h-96">
        <div class="text-gray-500">Loading...</div>
      </div>

      <div v-else-if="scatterError" class="bg-red-50 border border-red-200 rounded-md p-4">
        <p class="text-red-800">Error: {{ scatterError }}</p>
      </div>

      <HeatmapChart
        v-else
        :data="sizeOverTimeHeatmap"
        :height="600"
        x-label="Time (seconds)"
        y-label="Size (Bytes)"
        x-unit="s"
        y-unit="B"
        :log-x="false"
        :log-y="true"
        :bins-x="50"
        :bins-y="50"
      />
    </PageSection>

    <!-- Lifetime over size - Heatmap -->
    <PageSection title="Lifetime over size">
      <div v-if="scatterLoading" class="chart-loading-state">
        <div class="loading-message">Loading...</div>
      </div>

      <div v-else-if="scatterError" class="chart-error-state">
        <p class="error-message">Error: {{ scatterError }}</p>
      </div>

      <HeatmapChart
        v-else
        :data="lifetimeOverSizeHeatmap"
        :height="600"
        x-label="Size (Bytes)"
        y-label="Lifetime (Cycles)"
        x-unit="B"
        y-unit="C"
        :log-x="true"
        :log-y="true"
        :bins-x="50"
        :bins-y="50"
        :log-color="true"
      />
    </PageSection>
  </div>
</template>

<script setup lang="ts">
import { computed } from 'vue'
import { useAllocSizeDistr } from '@/composables/useAllocSizeDistr'
import { useAllocSizeDistrHeatmaps } from '@/composables/useAllocSizeDistrHeatmaps'
import SummaryCard from '@/components/home/SummaryCard.vue'
import MostUsedSizesChart from '@/components/charts/MostUsedSizesChart.vue'
import Log2HistogramChart from '@/components/charts/Log2HistogramChart.vue'
import HeatmapChart from '@/components/charts/HeatmapChart.vue'
import PageSection from '@/components/core/PageSection.vue'

const {
  loading,
  error,
  scatterData,
  scatterLoading,
  scatterError,
  meanChunkSize,
  mostUsedSize,
  largestSize,
  mostUsedSizesData,
  histogramData,
} = useAllocSizeDistr()

const { sizeOverTimeHeatmap, lifetimeOverSizeHeatmap } = useAllocSizeDistrHeatmaps(
  computed(() => scatterData.value),
  50,
  50,
)

const summaryItems = computed(() => [
  { title: 'Mean size', value: meanChunkSize.value },
  { title: 'Most used size', value: mostUsedSize.value },
  { title: 'Largest', value: largestSize.value },
])
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

/* Chart states */
.chart-loading-state {
  display: flex;
  align-items: center;
  justify-content: center;
  height: 384px; // h-96 equivalent
}

.loading-message {
  color: $gray-500;
}

.chart-error-state {
  background: $color-error-bg;
  border: 1px solid $color-error-border;
  border-radius: $radius-md;
  padding: $spacing-md;
}

.error-message {
  color: #991b1b; // text-red-800 equivalent
  margin: 0;
}
</style>

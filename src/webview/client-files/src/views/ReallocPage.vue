<template>
  <div class="realloc-page">
    <!-- Loading State -->
    <div v-if="loading" class="loading-container">
      <p>Loading realloc data...</p>
    </div>

    <!-- Error State -->
    <div v-else-if="error" class="error-container">
      <h2>Error loading data</h2>
      <p>{{ error }}</p>
    </div>

    <!-- Data Display -->
    <div v-else-if="data.length > 0" class="content-container">
      <!-- Summary Cards -->
      <SummaryCard :items="summaryItems" />

      <!-- Most Used Realloc Sizes -->
      <PageSection title="Most Used Realloc Sizes">
        <VerticalBarChart :data="mostUsedBarData" :height="500" />
      </PageSection>

      <!-- Most Used Realloc Gaps (Delta Histogram) -->
      <PageSection title="Most Used Realloc Gaps">
        <ReallocGapVerticalBarChart :data="topDeltas" :height="400" sort-label="Sort values" />
      </PageSection>

      <!-- Realloc Gap Histogram (Log2) -->
      <PageSection title="Realloc Gap Histogram">
        <Log2HistogramChart :data="log2Histogram" :height="500" />
      </PageSection>

      <!-- Scatter Chart -->
      <PageSection title="Realloc Scatter Chart">
        <ScatterChart :data="scatterData" :height="500" />
      </PageSection>

      <!-- Realloc Bundle Chart -->
      <PageSection title="Realloc Chart">
        <ReallocBundleChart :data="bundleData" :diameter="960" />
      </PageSection>
    </div>
  </div>
</template>

<script setup lang="ts">
import { computed } from 'vue'
import { useRealloc } from '@/composables/useRealloc'
import { humanReadable } from '@/lib/helpers'
import VerticalBarChart from '@/components/charts/VerticalBarChart.vue'
import ReallocGapVerticalBarChart from '@/components/charts/ReallocGapVerticalBarChart.vue'
import Log2HistogramChart from '@/components/charts/Log2HistogramChart.vue'
import ScatterChart from '@/components/charts/ScatterChart.vue'
import ReallocBundleChart from '@/components/charts/ReallocBundleChart.vue'
import SummaryCard from '@/components/home/SummaryCard.vue'
import PageSection from '@/components/core/PageSection.vue'
import type { BarSeries } from '@/types/global-vars'

const {
  data,
  loading,
  error,
  mostUsed,
  meanDelta,
  totalCount,
  topMostUsed,
  topDeltas,
  log2Histogram,
} = useRealloc()

const summaryItems = computed(() => [
  { title: 'Most Used', value: mostUsed?.value?.label || 'N/A' },
  {
    title: 'Mean Jump Size',
    value: humanReadable(meanDelta.value, 1, 'B', true),
  },
  { title: 'Total Number', value: totalCount.value.toLocaleString() },
])

// Limit data for performance
// ScatterChart: top 500 points by count
const scatterData = computed(() => {
  return [...data.value].sort((a, b) => b.count - a.count).slice(0, 500)
})

// ReallocBundleChart: top 100 links by count
const bundleData = computed(() => {
  return [...data.value].sort((a, b) => b.count - a.count).slice(0, 100)
})

// Prepare data for bar chart
const mostUsedBarData = computed((): BarSeries[] => {
  if (topMostUsed.value.length === 0) return []

  return [
    {
      key: 'Realloc',
      color: '#4682b4',
      values: topMostUsed.value.map((entry) => ({
        name: entry.label,
        value: entry.count,
      })),
    },
  ]
})
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.realloc-page {
  margin: 0 auto;
}

.loading-container,
.error-container {
  padding: 2rem;
  text-align: center;
}

.error-container {
  color: $color-error;
}
</style>

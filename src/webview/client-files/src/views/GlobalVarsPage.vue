<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/views/GlobalVarsPage.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
|    AUTHOR   : Sébastien Valat (INRIA) - 2025
----------------------------------------------------------->
<template>
  <div class="global-vars-page">
    <!-- Loading State -->
    <div v-if="loading" class="loading-state">
      <div class="spinner-border" role="status">
        <span class="visually-hidden">Loading global variables data...</span>
      </div>
      <p class="loading-text">Loading global variables data...</p>
    </div>

    <!-- Error State -->
    <div v-else-if="error" class="alert alert-danger" role="alert">
      <h4 class="alert-heading">Error loading data</h4>
      <p>{{ error }}</p>
    </div>

    <!-- Main Content -->
    <div v-else class="content-container">
      <!-- Summary Section -->
      <GlobalVarsSummary
        label1="Global variables:"
        :value1="globalVariablesMem"
        label2="TLS variables:"
        :value2="tlsVariablesMem"
        label3="Active threads:"
        :value3="activeThreads"
      />

      <!-- Top 10 Global Variables -->
      <PageSection>
        <template #title> Top 10 <span>global variables</span></template>
        <TopGlobalVariablesList
          v-model:search-query="topListSearchQuery"
          :filtered-variables="topListVariables"
          :maxThreadCount="activeThreads"
        />
      </PageSection>

      <!-- Treemap by Binary -->
      <PageSection>
        <template #title> Distribution <span>over binaries</span></template>
        <GlobalVarsTreemapChart :data="dataForBinaryTreemap" :height="500" />
      </PageSection>

      <!-- Treemap by Variable -->
      <PageSection>
        <template #title> Distribution<span>over variables</span></template>
        <GlobalVarsTreemapChart :data="dataForVariableTreemap" :height="500" />
      </PageSection>

      <!-- Binary Bar Chart -->
      <PageSection>
        <template #title> Distribution <span>over binaries (bar chart)</span></template>
        <HorizontalBarChart
          :data="dataForBinaryBar"
          :height="400"
          @value-click="onVarChartValueClick"
        />
      </PageSection>

      <!-- Variable Bar Chart -->
      <PageSection>
        <template #title> Distribution <span>over variables (bar chart)</span></template>
        <HorizontalBarChart :data="dataForVarBar" :height="400" />
      </PageSection>
    </div>
  </div>
</template>

<script setup lang="ts">
import { onMounted } from 'vue'
import { useGlobalVars } from '@/composables/useGlobalVars'
import GlobalVarsTreemapChart from '@/components/charts/GlobalVarsTreemapChart.vue'
import HorizontalBarChart from '@/components/charts/HorizontalBarChart.vue'
import GlobalVarsSummary from '@/components/shared/GlobalVarsSummary.vue'
import PageSection from '@/components/core/PageSection.vue'
import TopGlobalVariablesList from '@/components/home/TopGlobalVariablesList.vue'

const {
  loading,
  error,
  globalVariablesMem,
  tlsVariablesMem,
  activeThreads,
  dataForBinaryTreemap,
  dataForVariableTreemap,
  dataForBinaryBar,
  dataForVarBar,
  onVarChartValueClick,
  initializeVarBarData,
  topListSearchQuery,
  topListVariables,
} = useGlobalVars()

// Initialize variable bar chart data on mount
onMounted(() => {
  initializeVarBarData()
})
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.global-vars-page {
  margin: 0 auto;
}

/* Loading state */
.loading-state {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  padding: $spacing-xl 0;
  text-align: center;
  background-color: $blue-section-bg;
  border-radius: $radius-xl;
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
  color: $gray-600;
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
</style>

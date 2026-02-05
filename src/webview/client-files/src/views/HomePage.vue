<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/views/HomePage.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
|    AUTHOR   : Sébastien Valat (INRIA) - 2025
----------------------------------------------------------->
<template>
  <div class="home-page">
    <!-- Loading State -->
    <div v-if="loading" class="loading-state">
      <div class="spinner-border" role="status">
        <span class="visually-hidden">Loading...</span>
      </div>
      <p class="loading-text">Loading dashboard data...</p>
    </div>

    <!-- Error State -->
    <div v-else-if="error" class="alert alert-danger" role="alert">
      <h4 class="alert-heading">Error loading data</h4>
      <p>{{ error }}</p>
    </div>

    <!-- Main Content -->
    <div v-else-if="data" class="home-content">
      <!-- Run informations Section -->
      <PageSection>
        <template #title>Run <span>informations</span></template>

        <InfoCard title="Run summary" :icon="IconTimer" main>
          <SummaryMetric name='peakPhysicalMemory' title='Physical memory peak'></SummaryMetric>
          <div class="info-row">
            <div class="info-label">Available physical memory</div>
            <div class="info-value">{{ formattedTotalMemory }}</div>
          </div>
          <SummaryMetric name='allocCount' title='Allocation count'></SummaryMetric>
        </InfoCard>

        <InfoCard title="Run details" :icon="IconList">
          <div class="info-row">
            <div class="info-label">Executable</div>
            <div class="info-value">{{ data.run.exe }}</div>
          </div>
          <div class="info-row">
            <div class="info-label">Date</div>
            <div class="info-value">{{ data.run.date }}</div>
          </div>
          <div class="info-row">
            <div class="info-label">Execution time</div>
            <div class="info-value">{{ formattedExecTime }}</div>
          </div>
        </InfoCard>

        <InfoCard title="Device" :icon="IconDevice">
          <div class="info-row">
            <div class="info-label">Host</div>
            <div class="info-value">{{ data.run.hostname || 'N/A' }}</div>
          </div>
          <div class="info-row">
            <div class="info-label">Ticks frequency</div>
            <div class="info-value">{{ formattedCpuFreq }}</div>
          </div>
          <div class="info-row">
            <div class="info-label">Available physical memory</div>
            <div class="info-value">{{ formattedTotalMemory }}</div>
          </div>
        </InfoCard>

        <InfoCard class="full-width" title="Commande" :icon="IconScreen">
          <div class="info-row">
            <div class="info-label">Commande</div>
            <div class="info-value command">{{ data.run.command }}</div>
          </div>
        </InfoCard>

        <InfoCard
          class="full-width"
          v-if="data.run.tool || allocator || allocatorWrappers"
          title="Software context"
          :icon="IconGears"
        >
          <div v-if="data.run.tool" class="info-row">
            <div class="info-label">Tool</div>
            <div class="info-value">{{ data.run.tool }}</div>
          </div>
          <div v-if="allocator" class="info-row">
            <div class="info-label">Allocator used</div>
            <div class="info-value">{{ allocator }}</div>
          </div>
          <div v-if="allocatorWrappers" class="info-row">
            <div class="info-label">Custom allocators wrapped</div>
            <div class="info-value">{{ allocatorWrappers }}</div>
          </div>
        </InfoCard>

        <InfoCard class="full-width" title="Current profile" :icon="IconList">
          <div class="info-row">
            <div class="info-label">Current profile</div>
            <div class="info-value command">{{ data.profile.filename }}</div>
          </div>
        </InfoCard>
      </PageSection>

      <!-- Global statistics Section -->
      <PageSection class="four-columns" show-help>
        <template #title>Global <span>statistics</span></template>
        <template #action>
          <button class="show-help-btn" @click="showHelpModal = true">
            <IconInfo class="help-btn-icon" />
            <span>Show Help</span>
          </button>
        </template>

        <!-- Peak Statistics -->
        <InfoCard title="Peak" :icon="IconRules">
          <SummaryMetric name='peakPhysicalMemory' title='Physical memory peak'></SummaryMetric>
          <SummaryMetric name='peakVirtualMemory' title='Virtual memory peak'></SummaryMetric>
          <SummaryMetric name='peakRequestedMemory' title='Requested memory peak'></SummaryMetric>
          <SummaryMetric name='peakSegmentCount' title='Alive object peak'></SummaryMetric>
        </InfoCard>

        <!-- Allocator Pressure (card 1) -->
        <InfoCard title="Allocator pressure" :icon="IconWeight">
          <SummaryMetric name='totalAllocatedMemory' title='Cumulated memory allocations'></SummaryMetric>
          <SummaryMetric name='allocCount' title='Allocation count'></SummaryMetric>
        </InfoCard>

        <!-- Allocator Pressure (card 2) -->
        <InfoCard title="Allocator pressure" :icon="IconChrono">
          <SummaryMetric name='peakAllocRate' title='Peak allocation rate (volume)'></SummaryMetric>
          <SummaryMetric name='peakAllocCountRate' title='Peak allocation rate (count)'></SummaryMetric>
          <SummaryMetric name='recyclingRatio' title='Recycling ratio'></SummaryMetric>
        </InfoCard>

        <!-- Call stacks -->
        <InfoCard title="Call stacks" :icon="IconStack">
          <SummaryMetric name='maxThreadCount' title='Number of threads'></SummaryMetric>
          <SummaryMetric name='largestStack' title='Largest stack'></SummaryMetric>
        </InfoCard>

        <!-- Variables globales / TLS -->
        <InfoCard title="Variables globales / TLS" :icon="IconVariable">
          <SummaryMetric name='globalVarMem' title='Global variables memory'></SummaryMetric>
          <SummaryMetric name='tlsVarMem' title='TLS variables memory'></SummaryMetric>
          <SummaryMetric name='numGblVar' title='Global variable count'></SummaryMetric>
        </InfoCard>

        <!-- Memory leak -->
        <InfoCard title="Memory leak" :icon="IconWater">
          <SummaryMetric name='leakedMem' title='Leaked memory'></SummaryMetric>
          <SummaryMetric name='leakedCount' title='Leaked object count'></SummaryMetric>
        </InfoCard>

        <!-- Block sizes used -->
        <InfoCard title="Block sizes used" :icon="IconScale">
          <SummaryMetric name='minAllocSize' title='Smallest allocations'></SummaryMetric>
          <SummaryMetric name='meanAllocSize' title='Mean allocations'></SummaryMetric>
          <SummaryMetric name='maxAllocSize' title='Largest allocations'></SummaryMetric>
        </InfoCard>

        <!-- MALT -->
        <InfoCard title="MALT" :icon="IconSearch">
          <SummaryMetric name='peakInternalMemory' title='MALT peak memory'></SummaryMetric>
        </InfoCard>
      </PageSection>

      <!-- Domains Section -->
      <PageSection v-if="data.summaryDomains">
        <template #title><span>Domains</span></template>

        <!-- C/C++/Fortran/Rust -->
        <InfoCard :icon="IconC">
          <div class="info-row">
            <div class="info-label">Count</div>
            <div class="info-value">{{ data.summaryDomains.counters.c.toLocaleString() }}</div>
          </div>
          <div class="info-row">
            <div class="info-label">Cumulated allocations</div>
            <div class="info-value">{{ formatBytes(data.summaryDomains.mem.c) }}</div>
          </div>
          <div class="info-row">
            <div class="info-label">Description</div>
            <div class="info-value">
              The memory number of allocations made in C/C++/Fortran/Rust code.
            </div>
          </div>
        </InfoCard>

        <!-- Python -->

        <InfoCard :icon="IconPython">
          <template #action>
            <button class="show-help-btn" @click="showPythonDetailsModal = true">
              <IconInfo class="arrow-icon" />
              <span>Details</span>
            </button>
          </template>
          <div class="info-row">
            <div class="info-label">Count</div>
            <div class="info-value">
              {{
                (
                  data.summaryDomains.counters.pyObj +
                  data.summaryDomains.counters.pyMem +
                  data.summaryDomains.counters.pyRaw
                ).toLocaleString()
              }}
            </div>
          </div>
          <div class="info-row">
            <div class="info-label">Cumulated allocations</div>
            <div class="info-value">
              {{
                formatBytes(
                  data.summaryDomains.mem.pyObj +
                    data.summaryDomains.mem.pyMem +
                    data.summaryDomains.mem.pyRaw,
                )
              }}
            </div>
          </div>
          <div class="info-row">
            <div class="info-label">Description</div>
            <div class="info-value">The memory number of allocations made in Python code.</div>
          </div>
        </InfoCard>

        <!-- MMap -->
        <InfoCard title="MMap">
          <div class="info-row">
            <div class="info-label">Count</div>
            <div class="info-value">{{ data.summaryDomains.counters.mmap.toLocaleString() }}</div>
          </div>
          <div class="info-row">
            <div class="info-label">Cumulated allocations</div>
            <div class="info-value">{{ formatBytes(data.summaryDomains.mem.mmap) }}</div>
          </div>
          <div class="info-row">
            <div class="info-label">Description</div>
            <div class="info-value">
              The memory directory allocated by the mmap/munmap/mremap calls.
            </div>
          </div>
        </InfoCard>
      </PageSection>

      <!-- Top Functions Grid (2 sections per row) -->
      <div class="top-functions-grid">
        <!-- Allocation count -->
        <PageSection class="grid-section" action-inline>
          <template #title>Allocation <span>count</span></template>
          <template #action>
            <button class="show-help-btn" @click.prevent="openMetricHelp('alloc.count')">
              <IconArrow /> <span>Show Metric Help</span>
            </button>
          </template>
          <FunctionRankingList :items="topAllocCount" />
        </PageSection>

        <!-- Cumulated allocations -->
        <PageSection class="grid-section" action-inline>
          <template #title>Cumulated <span>allocations</span></template>
          <template #action>
            <button class="show-help-btn" @click.prevent="openMetricHelp('alloc.sum')">
              <IconArrow /> <span>Show Metric Help</span>
            </button>
          </template>
          <FunctionRankingList :items="topAllocSum" />
        </PageSection>

        <!-- Memory usage on peak -->
        <PageSection class="grid-section" action-inline>
          <template #title>Memory <span>usage on peak</span></template>
          <template #action>
            <button class="show-help-btn" @click.prevent="openMetricHelp('peakmem.global')">
              <IconArrow /> <span>Show Metric Help</span>
            </button>
          </template>
          <FunctionRankingList :items="topPeakMem" />
        </PageSection>

        <!-- Leaks -->
        <PageSection class="grid-section" action-inline>
          <template #title>Leaks</template>
          <template #action>
            <button class="show-help-btn" @click.prevent="openMetricHelp('leaks')">
              <IconArrow /> <span>Show Metric Help</span>
            </button>
          </template>
          <FunctionRankingList :items="topLeaks" />
        </PageSection>
      </div>
    </div>

    <!-- Help Modal -->
    <HelpModal v-model="showHelpModal" title-prefix="Global" title-highlight="statistics" />

    <!-- Python Details Modal -->
    <PythonDetailsModal
      v-if="data?.summaryDomains"
      v-model="showPythonDetailsModal"
      :python-data="data.summaryDomains"
    />

    <!-- Metric Help Modal -->
    <MetricHelpModal
      v-model="showMetricHelpModal"
      :metric-key="currentMetricKey"
      :metric-definitions="metricDefinitions"
    />
  </div>
</template>

<script setup lang="ts">
import { ref } from 'vue'
import { useHome } from '@/composables/useHome'
import PageSection from '@/components/core/PageSection.vue'
import InfoCard from '@/components/home/InfoCard.vue'
import FunctionRankingList from '@/components/home/FunctionRankingList.vue'
import WarningTooltip from '@/components/core/WarningTooltip.vue'
import HelpModal from '@/components/home/HelpModal.vue'
import PythonDetailsModal from '@/components/home/PythonDetailsModal.vue'
import MetricHelpModal from '@/components/home/MetricHelpModal.vue'
import SummaryMetric from '@/components/home/SummaryMetric.vue'
import IconTimer from '@/assets/icons/icon-timer.svg?component'
import IconList from '@/assets/icons/icon-list.svg?component'
import IconDevice from '@/assets/icons/icon-device.svg?component'
import IconWarning from '@/assets/icons/icon-warning.svg?component'
import IconInfo from '@/assets/icons/icon-info.svg?component'
import IconScreen from '@/assets/icons/icon-screen.svg?component'
import IconGears from '@/assets/icons/icon-gears.svg?component'
import IconRules from '@/assets/icons/icon-rules.svg?component'
import IconWeight from '@/assets/icons/icon-weight.svg?component'
import IconChrono from '@/assets/icons/icon-chrono.svg?component'
import IconStack from '@/assets/icons/icon-stack.svg?component'
import IconVariable from '@/assets/icons/icon-variable.svg?component'
import IconWater from '@/assets/icons/icon-water.svg?component'
import IconScale from '@/assets/icons/icon-scale.svg?component'
import IconSearch from '@/assets/icons/icon-search.svg?component'
import IconC from '@/assets/icons/c.svg?component'
import IconPython from '@/assets/icons/python.svg?component'
import IconArrow from '@/assets/icons/icon-arrow.svg?component'

// Home data
const {
  data,
  loading,
  error,
  formattedExecTime,
  formattedCpuFreq,
  formattedTotalMemory,
  allocatorWrappers,
  allocator,
  getFormattedValueFromKey,
  topAllocCount,
  topAllocSum,
  topPeakMem,
  topLeaks,
  metricDefinitions,
} = useHome()

// Helper to check if a key has warnings
const hasWarningForKey = (key: string): boolean => {
  if (!data.value?.summaryWarnings) return false
  const warnings = data.value.summaryWarnings[key]
  return warnings && warnings.length > 0
}

// Modal state
const showHelpModal = ref(false)
const showPythonDetailsModal = ref(false)
const showMetricHelpModal = ref(false)
const currentMetricKey = ref<string>('')

// Open metric help modal
const openMetricHelp = (metricKey: string) => {
  currentMetricKey.value = metricKey
  showMetricHelpModal.value = true
}

// Helper to get warning message for a specific key
const getWarningMessage = (key: string): string => {
  const value = data.value?.summaryWarnings[key] || ['Internal error !!!!']
  return value[0];
}

// Helper to format bytes with human-readable units
const formatBytes = (bytes: number): string => {
  if (bytes === 0) return '0 B'
  const k = 1024
  const sizes = ['B', 'KB', 'MB', 'GB', 'TB']
  const i = Math.floor(Math.log(bytes) / Math.log(k))
  return `${(bytes / Math.pow(k, i)).toFixed(1)} ${sizes[i]}`
}

// Top functions data comes from useHome composable
// topAllocCount, topAllocSum, topPeakMem, topLeaks
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.loading-state {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  height: 100vh;
  background-color: $blue-section-bg;
  border-radius: $radius-xl;
}

.home-content {
  margin: 0 auto;
}

/* Full width InfoCard */
.full-width {
  flex-basis: 100%;
  max-width: 100%;
}

/* Show help button */
.show-help-btn {
  display: flex;
  align-items: center;
  gap: 6px;
  background: transparent;
  color: $color-primary-light;
  border: none;
  border-radius: $radius-md;
  padding: 6px $spacing-md;
  font-family: $font-family-base;
  font-size: $font-size-base;
  font-weight: $font-weight-normal;
  cursor: pointer;
  transition: all $transition-base ease;
}

.show-help-btn:hover {
  background: rgba(59, 130, 246, 0.1);
}

.show-help-btn span {
  text-decoration: underline;
}

.help-btn-icon {
  width: 18px;
  height: 18px;
  flex-shrink: 0;
  color: $color-primary-light;
}

/* Warning icon */
.warning-icon {
  color: #aa2647;
  margin-left: $spacing-xs;
  width: $spacing-lg;
  height: $spacing-lg;
  display: inline-block;
  vertical-align: middle;
}

.info-value.warning {
  color: #aa2647;
}

/* Alert */
.alert {
  padding: 1rem;
  margin: 1rem;
  border: 1px solid transparent;
  border-radius: $radius-lg;
}

.alert-danger {
  color: #721c24;
  background-color: $color-error-bg;
  border-color: $color-error-border;
}

.alert-heading {
  margin-top: 0;
  margin-bottom: 0.5rem;
  font-family: $font-family-base;
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
  margin-top: 1rem;
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

.alert {
  margin: $spacing-md;
}

/* Info row styles */
.info-label {
  font-family: $font-family-base;
  font-weight: $font-weight-medium;
  font-size: $font-size-xs;
  line-height: 100%;
  color: $color-text-primary;
}

.info-value {
  font-family: $font-family-base;
  font-weight: $font-weight-bold;
  font-size: $font-size-md;
  line-height: 20px;
  color: $color-text-primary;
}

.info-value.command {
  border: 1px solid $color-border-light;
  background: linear-gradient(90.06deg, $color-bg-light 0%, $color-bg-lighter 100%);
  border-radius: $radius-md;
  padding: $spacing-sm;
  word-break: break-all;
}

/* Top Functions Grid - 2 sections per row */
.top-functions-grid {
  display: grid;
  grid-template-columns: repeat(2, 1fr);
  gap: $spacing-md;
  margin: 0 auto;
}

.top-functions-grid .grid-section {
  margin: 0;
  min-width: 0;
  overflow: hidden;
}

/* Metric help button */
.metric-help-btn {
  display: flex;
  align-items: center;
  gap: 6px;
  background: transparent;
  color: $color-primary-light;
  border: none;
  border-radius: $radius-md;
  padding: 6px $spacing-md;
  font-family: $font-family-base;
  font-size: $font-size-sm;
  font-weight: $font-weight-medium;
  cursor: pointer;
  transition: all $transition-base ease;
}

.metric-help-btn:hover {
  background: rgba(59, 130, 246, 0.1);
}

.metric-help-btn .arrow-icon {
  width: $spacing-lg;
  height: $spacing-lg;
  flex-shrink: 0;
}

.metric-help-btn .arrow-icon :deep(svg) {
  width: 100%;
  height: 100%;
}

/* Responsive */
@media (max-width: 1024px) {
  .top-functions-grid {
    grid-template-columns: 1fr;
  }
}

@media (max-width: 768px) {
  .home-content {
    padding: 0 $spacing-sm;
  }

  .top-functions-grid {
    padding: 0 $spacing-sm;
    gap: $spacing-lg;
  }
}
</style>

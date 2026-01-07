<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/core/MetricSelector.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->
<template>
  <div class="metric-selector">
    <!-- Toolbar buttons -->
    <div class="button-group">
      <!-- Sort order toggle -->
      <button type="button" class="btn" title="Sort order" @click="emit('toggleOrder')">
        <IconSortingDown :class="{ rotate: props.isAscending }" />
      </button>

      <!-- Ratio toggle -->
      <button
        type="button"
        class="btn"
        :class="{ active: props.ratio }"
        title="Show ratio to reference (max or sum) or absolute value"
        @click="emit('toggleRatio')"
      >
        <IconPercent />
      </button>

      <!-- Inclusive toggle -->
      <button
        type="button"
        class="btn"
        :class="{ active: props.inclusive }"
        title="Show inclusive or exclusive costs"
        @click="emit('toggleInclusive')"
      >
        <IconInclusive />
      </button>

      <!-- Metric dropdown -->
      <div class="dropdown">
        <button
          type="button"
          class="btn dropdown-toggle"
          title="Select metric"
          @click="toggleDropdown"
        >
          <span>{{ props.currentMetricName }}</span>
          <span class="caret">
            <IconChevronDown :class="{ open: isDropdownOpen }" />
          </span>
        </button>
        <ul v-if="isDropdownOpen" class="dropdown-menu">
          <li v-for="category in metricList" :key="category.label" class="dropdown-category">
            <ul>
              <li class="dropdown-header">{{ category.label }}</li>
              <li
                class="dropdown-item"
                v-for="metric in category.metrics"
                :key="metric.key"
                @click="selectMetric(metric.key)"
              >
                <a href="#" @click.prevent>{{ metric.name }}</a>
              </li>
            </ul>
          </li>
        </ul>
      </div>
    </div>

    <!-- Search input -->
    <div class="search-group">
      <input
        type="text"
        class="form-control"
        placeholder="Search"
        :value="props.query"
        @input="emit('update:query', ($event.target as HTMLInputElement).value)"
      />
      <IconSearch class="search-icon" />
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref } from 'vue'
import { getMetricList } from '@/utils/metrics'
import type { MetricKey } from '@/types/sources'
import IconSortingDown from '@/assets/icons/icon-sorting-down.svg?component'
import IconPercent from '@/assets/icons/icon-percent.svg?component'
import IconInclusive from '@/assets/icons/icon-inclusive.svg?component'
import IconChevronDown from '@/assets/icons/icon-chevron-down.svg?component'
import IconSearch from '@/assets/icons/icon-search.svg?component'

interface Props {
  currentMetricName: string
  isAscending: boolean
  ratio: boolean
  inclusive: boolean
  query: string
}

interface Emits {
  (e: 'toggleOrder'): void
  (e: 'toggleRatio'): void
  (e: 'toggleInclusive'): void
  (e: 'selectMetric', metric: MetricKey): void
  (e: 'update:query', value: string): void
}

const props = defineProps<Props>()
const emit = defineEmits<Emits>()

// Dropdown state
const isDropdownOpen = ref(false)

// Get metric list
const metricList = getMetricList()

const toggleDropdown = () => {
  isDropdownOpen.value = !isDropdownOpen.value
}

const selectMetric = (metric: MetricKey) => {
  emit('selectMetric', metric)
  isDropdownOpen.value = false
}

// Close dropdown when clicking outside
if (typeof window !== 'undefined') {
  window.addEventListener('click', (e) => {
    const target = e.target as HTMLElement
    if (!target.closest('.dropdown')) {
      isDropdownOpen.value = false
    }
  })
}
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.metric-selector {
  display: flex;
  flex-direction: column;
  gap: $spacing-lg;
}

.button-group {
  display: flex;
  gap: 2px;
}

.btn {
  width: 40px;
  height: 36px;
  background: $color-primary;
  color: white;
  cursor: pointer;
  transition: all $transition-fast;
  border-radius: $radius-md;
  display: flex;
  justify-content: center;
  align-items: center;
  border: none;
}

.btn :deep(svg) {
  transition: transform $transition-base ease;
}

.btn :deep(svg.rotate) {
  transform: rotate(180deg);
}

.btn:hover {
  opacity: 0.85;
}

.btn.active {
  background: #007bff;
  color: white;
  border-color: #007bff;
}

.dropdown {
  position: relative;
  flex: 1;
}

.dropdown-toggle {
  display: flex;
  align-items: center;
  gap: 0.5rem;
  width: 100%;
  justify-content: space-between;
  padding: 0 $spacing-md;
  font-size: $font-size-base;
  line-height: 20px;
  font-weight: $font-weight-medium;
  text-align: left;
  font-family: $font-family-base;
}

.caret {
  font-size: 0.75rem;
  display: flex;
  align-items: center;
  transition: transform $transition-base ease;
}

.caret :deep(svg) {
  transition: transform $transition-base ease;
}

.caret :deep(svg.open) {
  transform: rotate(180deg);
}

.dropdown-menu {
  position: absolute;
  top: 100%;
  left: 0;
  z-index: 1000;
  margin: 0.25rem 0 0;
  padding: 0 4px;
  list-style: none;
  background: $color-bg-primary;
  border-radius: $radius-md;
  box-shadow: 0px 2px 8px 0px #102d4129;
  border: 1px solid $color-primary-light;
  max-height: 500px;
  overflow-y: auto;
  width: calc(100% - 10px);
}

/* Custom scrollbar styling */

.dropdown-menu::-webkit-scrollbar-thumb {
  background: $color-primary;
  border-radius: $spacing-xs;
}

.dropdown-menu::-webkit-scrollbar-thumb:hover {
  background: $color-primary-light;
}

/* Firefox scrollbar */
.dropdown-menu {
  scrollbar-width: thin;
  scrollbar-color: $color-primary transparent;
}

.dropdown-header {
  font-size: $font-size-xs;
  font-weight: $font-weight-bold;
  color: $color-text-primary;
  margin-bottom: $spacing-xs;
  padding: 0 $spacing-sm;
}

.dropdown-category {
  padding: $spacing-md 0;
}

.dropdown-category ul {
  list-style: none;
  padding: 0;
}

.dropdown-category + .dropdown-category {
  border-top: 1px solid $color-primary-light;
}

.dropdown-item {
  font-size: $font-size-base;
  font-weight: $font-weight-normal;
}

.dropdown-item a {
  display: block;
  transition: $transition-base all;
  padding: $spacing-xs $spacing-sm;
  border-radius: $spacing-xs;
  font-size: 15px;
  color: $color-text-primary;
  text-decoration: none;
}

.dropdown-item a:hover {
  background: $color-bg-lighter;
}
.search-group {
  position: relative;
  display: flex;
}

.form-control {
  display: flex;
  border-radius: $radius-md;
  border-width: 1px;
  padding: $spacing-sm 40px $spacing-sm $spacing-md;
  gap: $spacing-sm;
  width: 100%;
  background: $color-bg-primary;
  border: 1px solid $color-border-medium;
  color: $color-primary;
  line-height: 20px;
  font-size: $font-size-base;
  font-weight: $font-weight-medium;
}

.search-icon {
  position: absolute;
  right: $spacing-md;
  top: 50%;
  transform: translateY(-50%);
  width: 20px;
  height: 20px;
  color: $color-primary;
  opacity: 0.6;
  pointer-events: none;
}

.form-control::placeholder {
  color: $color-primary;
  opacity: 0.6;
}

.form-control:focus {
  outline: none;
  border-color: $color-primary-light;
}
</style>

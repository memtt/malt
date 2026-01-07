<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/shared/FunctionList.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->
<template>
  <div class="function-list-wrapper">
    <!-- Optional Header -->
    <div
      v-if="showHeader && displayedFunctions.length > 0"
      class="function-list-header"
      :style="{ gridTemplateColumns: headerGridColumns }"
    >
      <div class="text-left">{{ headerValueLabel || 'Value / Ratio' }}</div>
      <div class="text-left">{{ headerFunctionLabel || 'Function' }}</div>
    </div>

    <!-- Function List -->
    <div v-if="displayedFunctions.length > 0" class="function-list">
      <div
        v-for="func in displayedFunctions"
        :key="func.function"
        class="function-item"
        :class="{ selected: isSelected(func) }"
        :style="{ gridTemplateColumns: itemGridColumns }"
        @click="handleFunctionClick(func)"
      >
        <!-- Value and Progress Bar -->
        <div class="value-column" :style="{ width: valueColumnWidth }">
          <div class="value-text">
            {{ getFormattedValue(func) }}
          </div>
          <div class="progress-bar-wrapper">
            <div
              class="progress-bar"
              :style="{ width: `${getLocalRatio(func)}%` }"
            ></div>
          </div>
        </div>

        <!-- Function Name with Tooltip -->
        <TooltipWrapper
          :content="compact ? (func.functionShort || func.function) : func.function"
        >
          <component
            :is="useLinkNavigation ? 'router-link' : 'span'"
            :to="useLinkNavigation ? getFunctionLink(func) : undefined"
            class="function-link"
            :class="{ clickable: !useLinkNavigation }"
          >
            {{ compact ? (func.functionShort || func.function) : func.function }}
          </component>
        </TooltipWrapper>
      </div>
    </div>

    <!-- Empty State -->
    <div v-else class="empty-state">
      {{ emptyMessage || 'No data available' }}
    </div>

    <!-- Pagination -->
    <BasePagination
      v-if="showPagination && totalPages > 1"
      :current-page="currentPage"
      :total-pages="totalPages"
      @update:current-page="emit('update:currentPage', $event)"
    />
  </div>
</template>

<script setup lang="ts">
import { computed } from 'vue'
import type { FunctionStat } from '@/types/sources'
import BasePagination from '@/components/core/BasePagination.vue'
import TooltipWrapper from '@/components/core/TooltipWrapper.vue'

interface Props {
  functions: FunctionStat[] // Filtered functions
  allFunctions: FunctionStat[] // All functions (unfiltered) for progress bar reference
  getFormattedValue: (func: FunctionStat) => string
  getValueRatio?: (func: FunctionStat) => number
  getValue: (func: FunctionStat) => number
  isReversedOrder?: boolean
  currentPage?: number
  perPage?: number
  selectedFunction?: string
  compact?: boolean
  showHeader?: boolean
  showPagination?: boolean
  headerValueLabel?: string
  headerFunctionLabel?: string
  emptyMessage?: string
  useLinkNavigation?: boolean
  getLinkForFunction?: (func: FunctionStat) => string
  valueColumnWidth?: string
  headerGridColumns?: string
  itemGridColumns?: string
}

interface Emits {
  (e: 'select', func: FunctionStat): void
  (e: 'update:currentPage', page: number): void
}

const props = withDefaults(defineProps<Props>(), {
  compact: true,
  showHeader: false,
  showPagination: true,
  isReversedOrder: true,
  currentPage: 1,
  perPage: 20,
  useLinkNavigation: false,
  valueColumnWidth: '80px',
  headerGridColumns: '150px 1fr',
  itemGridColumns: '80px 1fr',
})

const emit = defineEmits<Emits>()

// Sorted functions
const sortedFunctions = computed(() => {
  return [...props.functions].sort((a, b) => {
    const valueA = props.getValue(a)
    const valueB = props.getValue(b)

    if (props.isReversedOrder) {
      return valueB - valueA // Descending
    } else {
      return valueA - valueB // Ascending
    }
  })
})

// Paginated functions
const paginatedFunctions = computed(() => {
  if (!props.showPagination) {
    return sortedFunctions.value
  }
  const start = (props.currentPage - 1) * props.perPage
  const end = start + props.perPage
  return sortedFunctions.value.slice(start, end)
})

// Displayed functions
const displayedFunctions = computed(() => {
  return paginatedFunctions.value
})

// Total pages
const totalPages = computed(() => {
  return Math.ceil(props.functions.length / props.perPage)
})

// Calculate max value across ALL functions (unfiltered)
// This ensures progress bars stay absolute and never change with search/filter
const maxValueInAllFunctions = computed(() => {
  if (props.allFunctions.length === 0) return 1
  return Math.max(...props.allFunctions.map((f) => props.getValue(f)), 1)
})

// Get local ratio based on maximum of ALL functions (unfiltered)
// This keeps bars absolute - they never change with search or pagination
const getLocalRatio = (func: FunctionStat): number => {
  const value = props.getValue(func)
  return (100 * value) / maxValueInAllFunctions.value
}

// Check if function is selected
const isSelected = (func: FunctionStat): boolean => {
  return func.function === props.selectedFunction
}

// Get function link
const getFunctionLink = (func: FunctionStat): string => {
  if (props.getLinkForFunction) {
    return props.getLinkForFunction(func)
  }
  return '#'
}

// Handle function click
const handleFunctionClick = (func: FunctionStat) => {
  if (!props.useLinkNavigation) {
    emit('select', func)
  }
}
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

// Wrapper
.function-list-wrapper {
  width: 100%;
}

// Header
.function-list-header {
  display: grid;
  gap: $spacing-md;
  border-radius: $radius-xl;
  background: $color-bg-lighter;
  padding: $spacing-sm $spacing-md;
  font-size: 13px;
  font-weight: $font-weight-medium;
  color: $color-primary-dark;
}

// Function List
.function-list {
  display: flex;
  flex-direction: column;
  gap: 4px;
  padding: $spacing-md 0;
}

// Function Item
.function-item {
  display: grid;
  align-items: center;
  gap: $spacing-md;
  border-radius: $radius-xl;
  border: 1px solid $color-border-light;
  background: $color-bg-primary;
  padding: $spacing-sm $spacing-sm $spacing-sm $spacing-md;
  transition: all $transition-fast $transition-ease;

  &.selected {
    background: $gradient-card;

    .function-link {
      color: $color-text-primary;
      font-weight: $font-weight-bold;
      text-decoration: none;
    }
  }
}

// Value Column
.value-column {
  display: flex;
  flex-direction: column;
  gap: $spacing-xs;
}

.value-text {
  text-align: right;
  font-size: $font-size-xs;
  font-weight: $font-weight-medium;
  line-height: 1;
  color: $color-primary-dark;
}

// Progress Bar
.progress-bar-wrapper {
  width: 100%;
  overflow: hidden;
  border-radius: $radius-full;
  border: 1px solid #c2e3f5;
  background: $color-bg-lighter;
  padding: 1px;
}

.progress-bar {
  height: 6px;
  border-radius: $radius-full;
  background: $color-primary;
  transition: width $transition-slow $transition-ease;
}

// Function Link
.function-link {
  display: block;
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
  font-family: $font-family-base;
  font-weight: $font-weight-normal;
  font-size: $font-size-base;
  line-height: 100%;
  color: $color-primary-light;
  text-decoration: underline;
  text-decoration-style: solid;
  transition: color $transition-base ease;

  &:hover {
    color: #155f8a;
  }

  &.clickable {
    cursor: pointer;
  }
}

// Empty State
.empty-state {
  padding: $spacing-xl;
  text-align: center;
  font-style: italic;
  color: $gray-500;
}
</style>

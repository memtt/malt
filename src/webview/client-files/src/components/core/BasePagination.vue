<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/core/BasePagination.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->
<template>
  <div v-if="props.totalPages > 1" class="pagination">
    <!-- Previous button -->
    <button
      class="pagination-btn prev"
      :disabled="props.currentPage === 1"
      @click="goToPreviousPage"
      title="Previous page"
    >
      <IconChevronLeft />
    </button>

    <!-- Page info -->
    <div class="page-info">
      <span class="page-text"
        ><span>{{ props.currentPage }}</span> / {{ props.totalPages }}</span
      >
    </div>

    <!-- Next button -->
    <button
      class="pagination-btn next"
      :disabled="props.currentPage === props.totalPages"
      @click="goToNextPage"
      title="Next page"
    >
      <IconChevronRight />
    </button>

    <!-- Page input -->
    <div class="page-input-group">
      <label for="page-input" class="page-label">Page:</label>
      <input
        id="page-input"
        v-model.number="pageInput"
        type="number"
        min="1"
        :max="props.totalPages"
        class="page-input"
        @keyup.enter="goToPage"
      />
    </div>

    <!-- Go button -->
    <button class="pagination-btn go" @click="goToPage">Go</button>
  </div>
</template>

<script setup lang="ts">
import { ref, watch } from 'vue'
import IconChevronLeft from '@/assets/icons/icon-chevron-left.svg?component'
import IconChevronRight from '@/assets/icons/icon-chevron-right.svg?component'

interface Props {
  currentPage: number
  totalPages: number
}

interface Emits {
  (e: 'update:currentPage', page: number): void
}

const props = defineProps<Props>()
const emit = defineEmits<Emits>()

const pageInput = ref(props.currentPage)

// Watch currentPage to sync with input
watch(
  () => props.currentPage,
  (newPage) => {
    pageInput.value = newPage
  },
)

const goToPreviousPage = () => {
  if (props.currentPage > 1) {
    emit('update:currentPage', props.currentPage - 1)
  }
}

const goToNextPage = () => {
  if (props.currentPage < props.totalPages) {
    emit('update:currentPage', props.currentPage + 1)
  }
}

const goToPage = () => {
  const page = pageInput.value
  if (page >= 1 && page <= props.totalPages) {
    emit('update:currentPage', page)
  } else {
    // Reset to current page if invalid
    pageInput.value = props.currentPage
  }
}
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.pagination {
  display: flex;
  align-items: center;
  gap: $spacing-sm;
  margin-top: $spacing-xl;
}

.pagination-btn {
  display: flex;
  align-items: center;
  justify-content: center;
  width: 36px;
  height: 36px;
  background: $color-primary;
  color: white;
  border: none;
  border-radius: $radius-md;
  cursor: pointer;
  transition: all $transition-fast;
  font-size: $font-size-lg;
  font-weight: $font-weight-semibold;
}

.pagination-btn:hover:not(:disabled) {
  background: $color-primary-light;
}

.pagination-btn:disabled {
  opacity: 0.4;
  cursor: not-allowed;
}

.pagination-btn.go {
  width: 36px;
  height: 36px;
  font-size: $font-size-base;
  font-weight: $font-weight-medium;
}

.page-info {
  display: flex;
  align-items: center;
  padding: 0 $spacing-sm;
}

.page-text {
  font-size: $font-size-md;
  font-weight: $font-weight-normal;
  color: $color-primary;
}

.page-text span {
  font-weight: $font-weight-medium;
}

.page-input-group {
  display: flex;
  align-items: center;
  gap: $spacing-xs;
  margin-left: auto;
}

.page-label {
  font-size: $font-size-md;
  font-weight: $font-weight-normal;
  color: $color-primary;
}

.page-input {
  width: 50px;
  height: 34px;
  border: 1px solid $color-border-medium;
  border-radius: $radius-md;
  font-size: $font-size-base;
  font-weight: $font-weight-medium;
  color: $color-primary;
  text-align: center;
  background: $color-bg-primary;
}

.page-input:focus {
  outline: none;
  border-color: $color-primary-light;
}

/* Remove number input arrows */
.page-input::-webkit-inner-spin-button,
.page-input::-webkit-outer-spin-button {
  -webkit-appearance: none;
  margin: 0;
}

.page-input[type='number'] {
  -moz-appearance: textfield;
}
</style>

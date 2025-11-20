<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/shared/AllocatedChunksModal.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->
<template>
  <BaseModal
    :model-value="isOpen"
    title="Allocated Chunks"
    container-class="allocated-chunks-modal"
    @update:model-value="emit('close')"
  >
    <!-- Loading state -->
    <div v-if="loading" class="loading-state">
      <div class="spinner"></div>
      <p>Loading chunks data...</p>
    </div>

    <!-- Error state -->
    <div v-else-if="error" class="error-state">
      <p class="error-message">Error loading chunks: {{ error.message }}</p>
    </div>

    <!-- Data table -->
    <div v-else-if="chunks && chunks.result.length > 0" class="table-container">
      <table class="chunks-table">
        <thead>
          <tr>
            <th>Stack</th>
            <th>Count</th>
            <th>Total Size</th>
          </tr>
        </thead>
        <tbody>
          <tr v-for="(chunk, index) in chunks.result" :key="index">
            <td class="stack-cell">
              <span
                v-for="(stackItem, stackIndex) in chunk.stack"
                :key="stackIndex"
                class="stack-item"
              >
                <a
                  :href="`#/sources?func=${encodeURIComponent(stackItem.function)}&metric=alloc.count`"
                  class="function-link"
                >
                  <code>{{ stackItem.function }}</code>
                </a>
                <span v-if="stackIndex < chunk.stack.length - 1" class="chevron">›</span>
              </span>
            </td>
            <td class="count-cell">{{ formatNumber(chunk.chunkCount) }}</td>
            <td class="size-cell">{{ formatSize(chunk.chunkTotalSize) }}</td>
          </tr>
        </tbody>
      </table>
    </div>

    <!-- Empty state -->
    <div v-else class="empty-state">
      <p>No allocated chunks found at this timestamp.</p>
    </div>

    <template #footer>
      <button class="btn btn-default" @click="handleClose">Close</button>
    </template>
  </BaseModal>
</template>

<script setup lang="ts">
import BaseModal from '@/components/core/BaseModal.vue'
import type { ActiveChunksResponse } from '@/types/timeline'
import { humanReadable } from '@/lib/helpers'

interface Props {
  isOpen: boolean
  chunks: ActiveChunksResponse | null
  loading?: boolean
  error?: Error | null
}

const props = withDefaults(defineProps<Props>(), {
  loading: false,
  error: null,
})

const emit = defineEmits<{
  close: []
}>()

const handleClose = () => {
  emit('close')
}

const formatNumber = (value: number): string => {
  return value.toLocaleString()
}

const formatSize = (bytes: number): string => {
  return humanReadable(bytes, 1, 'B', true)
}
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.allocated-chunks-modal :deep(.modal-container) {
  width: 900px;
  max-width: 90%;
}

.loading-state,
.error-state,
.empty-state {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  padding: $spacing-3xl 20px;
  text-align: center;
}

.spinner {
  border: 3px solid $gray-100;
  border-top: 3px solid #2563eb;
  border-radius: 50%;
  width: 40px;
  height: 40px;
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

.error-message {
  color: $color-error;
  font-size: $font-size-sm;
}

.table-container {
  overflow-x: auto;
}

.chunks-table {
  width: 100%;
  border-collapse: collapse;
  font-size: 13px;
}

.chunks-table thead {
  background-color: $gray-50;
  border-bottom: 2px solid $color-border-gray;
}

.chunks-table th {
  padding: $spacing-md;
  text-align: left;
  font-weight: $font-weight-semibold;
  color: $color-text-dark;
  white-space: nowrap;
}

.chunks-table tbody tr {
  border-bottom: 1px solid $color-border-gray;
  transition: background-color $transition-fast;
}

.chunks-table tbody tr:hover {
  background-color: $gray-50;
}

.chunks-table td {
  padding: $spacing-md;
}

.stack-cell {
  max-width: 600px;
}

.stack-item {
  display: inline-flex;
  align-items: center;
  gap: $spacing-xs;
}

.function-link {
  color: #2563eb;
  text-decoration: none;
  transition: color $transition-base;
}

.function-link:hover {
  color: #1d4ed8;
  text-decoration: underline;
}

.function-link code {
  background-color: $gray-100;
  padding: 2px 6px;
  border-radius: 3px;
  font-size: $font-size-xs;
  font-family: $font-family-mono;
}

.chevron {
  color: $gray-300;
  font-size: $font-size-sm;
  margin: 0 $spacing-xs;
}

.count-cell,
.size-cell {
  text-align: right;
  font-variant-numeric: tabular-nums;
  white-space: nowrap;
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
  color: $color-text-dark;
  border-color: $gray-300;
}

.btn-default:hover {
  background-color: $gray-200;
}
</style>

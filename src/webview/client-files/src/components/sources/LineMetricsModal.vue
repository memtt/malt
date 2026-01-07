<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/sources/LineMetricsModal.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
|    AUTHOR   : Sébastien Valat (INRIA) - 2025
----------------------------------------------------------->
<template>
  <BaseModal
    v-model="isOpen"
    :title="modalTitle"
    container-class="metrics-modal-container"
    body-class="metrics-modal-body"
  >
    <!-- Point to Call Stack button -->
    <button class="show-help-btn" @click="handlePointToCallStack">
      <IconArrow class="help-btn-icon" />
      <span>Point Call Stack to here</span>
    </button>

    <div class="metrics-content">
      <!-- Inclusive metric -->
      <div class="metric-section">
        <h3 class="metric-section-title">Inclusive metric</h3>
        <div class="metric-grid">
          <div class="metric-row">
            <span class="metric-label">Allocated memory:</span>
            <span class="metric-value">{{ formatValue(annotation?.total?.alloc.sum, 'B') }}</span>
          </div>
          <div class="metric-row">
            <span class="metric-label">Freed memory:</span>
            <span class="metric-value">{{ formatValue(annotation?.total?.free.sum, 'B') }}</span>
          </div>
          <div class="metric-row">
            <span class="metric-label">Local peak:</span>
            <span class="metric-value">{{ formatValue(annotation?.total?.maxAliveReq, 'B') }}</span>
          </div>
          <div class="metric-row">
            <span class="metric-label">Leaks:</span>
            <span class="metric-value">{{ formatValue(annotation?.total?.aliveReq, 'B') }}</span>
          </div>
          <div class="metric-row">
            <span class="metric-label"
              >{{ formatValue(annotation?.total?.alloc.count, '') }} alloc [ Min. / Avg. / Max.
              ]:</span
            >
            <span class="metric-value">
              [ {{ formatValue(annotation?.total?.alloc.min, 'B') }} /
              {{ formatMean(annotation?.total?.alloc.sum, annotation?.total?.alloc.count, 'B') }} /
              {{ formatValue(annotation?.total?.alloc.max, 'B') }} ]
            </span>
          </div>
          <div class="metric-row">
            <span class="metric-label"
              >{{ formatValue(annotation?.total?.free.count, '') }} free [ Min. / Avg. / Max.
              ]:</span
            >
            <span class="metric-value">
              [ {{ formatValue(annotation?.total?.free.min, 'B') }} /
              {{ formatMean(annotation?.total?.free.sum, annotation?.total?.free.count, 'B') }} /
              {{ formatValue(annotation?.total?.free.max, 'B') }} ]
            </span>
          </div>
          <div class="metric-row">
            <span class="metric-label">Lifetime (cycles) [ Min. / Avg. / Max. ]:</span>
            <span class="metric-value">
              [ {{ formatValue(annotation?.total?.lifetime.min, 'C') }} /
              {{
                formatMean(annotation?.total?.lifetime.sum, annotation?.total?.lifetime.count, 'C')
              }}
              / {{ formatValue(annotation?.total?.lifetime.max, 'C') }} ]
            </span>
          </div>
        </div>
      </div>

      <!-- Exclusive metric -->
      <div class="metric-section">
        <h3 class="metric-section-title">Exclusive metric</h3>
        <div class="metric-grid">
          <div class="metric-row">
            <span class="metric-label">Allocated memory:</span>
            <span class="metric-value">{{ formatValue(annotation?.own?.alloc.sum, 'B') }}</span>
          </div>
          <div class="metric-row">
            <span class="metric-label">Freed memory:</span>
            <span class="metric-value">{{ formatValue(annotation?.own?.free.sum, 'B') }}</span>
          </div>
          <div class="metric-row">
            <span class="metric-label">Local peak:</span>
            <span class="metric-value">{{ formatValue(annotation?.own?.maxAliveReq, 'B') }}</span>
          </div>
          <div class="metric-row">
            <span class="metric-label">Leaks:</span>
            <span class="metric-value">{{ formatValue(annotation?.own?.aliveReq, 'B') }}</span>
          </div>
          <div class="metric-row">
            <span class="metric-label"
              >{{ formatValue(annotation?.own?.alloc.count, '') }} alloc [ Min. / Avg. / Max.
              ]:</span
            >
            <span class="metric-value">
              [ {{ formatValue(annotation?.own?.alloc.min, 'B') }} /
              {{ formatMean(annotation?.own?.alloc.sum, annotation?.own?.alloc.count, 'B') }} /
              {{ formatValue(annotation?.own?.alloc.max, 'B') }} ]
            </span>
          </div>
          <div class="metric-row">
            <span class="metric-label"
              >{{ formatValue(annotation?.own?.free.count, '') }} free [ Min. / Avg. / Max. ]:</span
            >
            <span class="metric-value">
              [ {{ formatValue(annotation?.own?.free.min, 'B') }} /
              {{ formatMean(annotation?.own?.free.sum, annotation?.own?.free.count, 'B') }} /
              {{ formatValue(annotation?.own?.free.max, 'B') }} ]
            </span>
          </div>
          <div class="metric-row">
            <span class="metric-label">Lifetime (cycles) [ Min. / Avg. / Max. ]:</span>
            <span class="metric-value">
              [ {{ formatValue(annotation?.own?.lifetime.min, 'C') }} /
              {{ formatMean(annotation?.own?.lifetime.sum, annotation?.own?.lifetime.count, 'C') }}
              / {{ formatValue(annotation?.own?.lifetime.max, 'C') }} ]
            </span>
          </div>
        </div>
      </div>
    </div>
  </BaseModal>
</template>

<script setup lang="ts">
import { computed } from 'vue'
import BaseModal from '@/components/core/BaseModal.vue'
import { humanReadable } from '@/lib/helpers'
import type { SourceAnnotation } from '@/types/sources'
import IconArrow from '@/assets/icons/icon-arrow.svg?component'

interface Props {
  modelValue: boolean
  annotation: SourceAnnotation | null
}

interface Emits {
  (e: 'update:modelValue', value: boolean): void
  (e: 'pointToCallStack'): void
}

const props = defineProps<Props>()
const emit = defineEmits<Emits>()

const isOpen = computed({
  get: () => props.modelValue,
  set: (value) => emit('update:modelValue', value),
})

const modalTitle = computed(() => {
  if (!props.annotation) return 'Line Metrics'
  const func = props.annotation.function || 'Unknown'
  return func
})

/**
 * Handle point to call stack button click
 */
const handlePointToCallStack = () => {
  emit('pointToCallStack')
  isOpen.value = false
}

/**
 * Format value with unit
 */
const formatValue = (value: number | undefined, unit: string): string => {
  if (value === undefined) {
    return humanReadable(0, 1, unit, true)
  }
  return humanReadable(value, 1, unit, true)
}

/**
 * Format mean (rounded ratio)
 */
const formatMean = (sum: number | undefined, count: number | undefined, unit: string): string => {
  if (sum === undefined || count === undefined || count === 0) {
    return humanReadable(0, 1, unit, true)
  }
  const mean = Math.round(sum / count)
  return humanReadable(mean, 1, unit, true)
}
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.metrics-modal-container {
  max-width: 1000px;
}

.metrics-modal-body {
  padding: 0 $spacing-3xl $spacing-3xl;
}

.metrics-content {
  display: flex;
  gap: $spacing-xl;
}

.metric-section {
  flex: 1;
  border-radius: $radius-lg;
  border: 1px solid $color-border-light;
  gap: $spacing-lg;
  padding: $spacing-lg;
  width: 350px;
  background: $gradient-card;
}

.metric-section-title {
  font-size: $font-size-md;
  font-weight: $font-weight-normal;
  margin: 0 0 $spacing-lg 0;
  color: $color-text-primary;
}

.metric-grid {
  display: flex;
  flex-direction: column;
  gap: $spacing-md;
}

.metric-row {
  display: flex;
  flex-direction: column;
  gap: 2px;
}

.metric-label {
  font-size: $font-size-xs;
  color: $color-text-primary;
  font-weight: $font-weight-medium;
}

.metric-value {
  font-size: $font-size-base;
  font-weight: $font-weight-bold;
  color: $color-text-primary;
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
  margin-bottom: $spacing-lg;
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
}

/* Responsive */
@media (max-width: 768px) {
  .metrics-content {
    flex-direction: column;
  }
}
</style>

<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/home/PythonDetailsModal.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->
<template>
  <BaseModal
    :model-value="props.modelValue"
    title="Python"
    container-class="python-details-modal"
    @update:model-value="$emit('update:modelValue', $event)"
  >
    <div class="cards-grid">
      <InfoCard title="Objects" main>
        <div class="info-row">
          <div class="info-label">Count:</div>
          <div class="info-value">{{ formatCount(props.pythonData.counters.pyObj) }}</div>
        </div>
        <div class="info-row">
          <div class="info-label">Cumulated allocations:</div>
          <div class="info-value">{{ formatBytes(props.pythonData.mem.pyObj) }}</div>
        </div>
        <div class="info-row">
          <div class="info-label">Description:</div>
          <div class="info-value">
            The memory number of allocations made in Python for objects. Note it uses a dedicated
            fast allocator.
          </div>
        </div>
      </InfoCard>

      <InfoCard title="Memory" main>
        <div class="info-row">
          <div class="info-label">Count:</div>
          <div class="info-value">{{ formatCount(props.pythonData.counters.pyMem) }}</div>
        </div>
        <div class="info-row">
          <div class="info-label">Cumulated allocations:</div>
          <div class="info-value">{{ formatBytes(props.pythonData.mem.pyMem) }}</div>
        </div>
        <div class="info-row">
          <div class="info-label">Description:</div>
          <div class="info-value">
            The memory number of allocations made in Python for medium sizes.
          </div>
        </div>
      </InfoCard>

      <InfoCard title="Raw" main>
        <div class="info-row">
          <div class="info-label">Count:</div>
          <div class="info-value">{{ formatCount(props.pythonData.counters.pyRaw) }}</div>
        </div>
        <div class="info-row">
          <div class="info-label">Cumulated allocations:</div>
          <div class="info-value">{{ formatBytes(props.pythonData.mem.pyRaw) }}</div>
        </div>
        <div class="info-row">
          <div class="info-label">Description:</div>
          <div class="info-value">
            The memory number of allocations made in Python for large allocations, sent directly to
            the C allocator.
          </div>
        </div>
      </InfoCard>
    </div>
  </BaseModal>
</template>

<script setup lang="ts">
import BaseModal from '@/components/core/BaseModal.vue'
import InfoCard from '@/components/home/InfoCard.vue'
import type { SummaryDomains } from '@/types/home'

interface Props {
  modelValue: boolean
  pythonData: SummaryDomains
}

const props = defineProps<Props>()

defineEmits<{
  'update:modelValue': [value: boolean]
}>()

const formatCount = (count: number): string => {
  return count.toLocaleString()
}

const formatBytes = (bytes: number): string => {
  if (bytes === 0) return '0 B'
  const k = 1024
  const sizes = ['B', 'KB', 'MB', 'GB', 'TB']
  const i = Math.floor(Math.log(bytes) / Math.log(k))
  return `${(bytes / Math.pow(k, i)).toFixed(1)} ${sizes[i]}`
}
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.python-details-modal :deep(.modal-container) {
  max-width: 960px;
}

.cards-grid {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 1rem;
}

@media (max-width: 1024px) {
  .cards-grid {
    grid-template-columns: 1fr;
  }
}

/* Info row styles */
.info-row {
  display: flex;
  flex-direction: column;
  gap: $spacing-xs;
  padding: 0;
  border-bottom: 1px solid $color-border-gray;
}

.info-row:last-child {
  border-bottom: none;
}

.info-label {
  font-family: $font-family-base;
  font-weight: $font-weight-medium;
  font-size: $font-size-xs;
  line-height: 100%;
  color: $color-text-primary;
}

.info-value {
  font-family: $font-family-base;
  font-weight: $font-weight-normal;
  font-size: $font-size-base;
  line-height: 140%;
  color: $color-text-primary;
}
</style>

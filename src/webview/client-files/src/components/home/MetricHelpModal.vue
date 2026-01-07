<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/home/MetricHelpModal.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->
<template>
  <BaseModal
    :model-value="props.modelValue"
    :title="metric?.label || 'Metric Help'"
    container-class="metric-help-modal"
    @update:model-value="$emit('update:modelValue', $event)"
  >
    <div v-if="metric" class="metric-help-content">
      <div class="metric-description">
        {{ metric.description }}
      </div>
    </div>
    <div v-else class="metric-help-content">
      <p class="no-metric">No metric information available.</p>
    </div>
  </BaseModal>
</template>

<script setup lang="ts">
import { computed } from 'vue'
import BaseModal from '@/components/core/BaseModal.vue'
import type { MetricDefinition } from '@/composables/useHome'

interface Props {
  modelValue: boolean
  metricKey?: string
  metricDefinitions?: Record<string, MetricDefinition>
}

const props = defineProps<Props>()

defineEmits<{
  'update:modelValue': [value: boolean]
}>()

const metric = computed(() => {
  if (!props.metricKey || !props.metricDefinitions) return null
  return props.metricDefinitions[props.metricKey] || null
})
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.metric-help-modal :deep(.modal-container) {
  max-width: 600px;
}

.metric-help-content {
  padding: $spacing-sm 0;
}

.metric-description {
  font-family: $font-family-base;
  font-size: $font-size-base;
  font-weight: $font-weight-normal;
  line-height: 160%;
  color: $color-text-dark;
}

.no-metric {
  font-family: $font-family-base;
  font-size: $font-size-sm;
  color: $color-text-muted;
  text-align: center;
  padding: $spacing-xl;
}
</style>

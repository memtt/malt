<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/home/SummaryMetric.vue
|-----------------------------------------------------------
|    AUTHOR   : Sébastien Valat (INRIA) - 2025
----------------------------------------------------------->
<template>
  <div class="info-row">
    <div class="info-label">{{ props.title }}</div>
      <div class="info-value" :class="{ warning: hasWarningForKey(props.name || 'unknown') }">
        {{ getFormattedValueFromKey(props.name || 'unknown') }}
        <WarningTooltip
        v-if="hasWarningForKey(props.name || 'unknown')"
        :message="getWarningMessage(props.name || 'unknown')"
        >
          <IconWarning class="warning-icon" />
        </WarningTooltip>
      </div>
    </div>
</template>

<script setup lang="ts">
import { useHome } from '@/composables/useHome'
import WarningTooltip from '@/components/core/WarningTooltip.vue'
import IconWarning from '@/assets/icons/icon-warning.svg?component'

interface Props {
  name?: string
  title?: string
}

const props = defineProps<Props>()
defineEmits<{
  name: "undefined",
  title: "undefined",
}>()

// Home data
const {
  data,
  getFormattedValueFromKey,
} = useHome()

// Helper to check if a key has warnings
const hasWarningForKey = (key: string): boolean => {
  if (!data.value?.summaryWarnings) return false
  const warnings = data.value.summaryWarnings[key]
  return warnings && warnings.length > 0
}

// Helper to format recycling ratio with 8 decimal places
const formatRecyclingRatio = (value: number | undefined): string => {
  if (value === undefined) return 'N/A'
  return value.toFixed(2)
}

// Helper to get warning message for a specific key
const getWarningMessage = (key: string): string => {
  const value = data.value?.summaryWarnings[key] || ['Internal error !!!!']
  return value[0];
}

// Top functions data comes from useHome composable
// topAllocCount, topAllocSum, topPeakMem, topLeaks

</script>

<style scoped lang="scss">

@use '@/assets/scss/variables' as *;

/* Info row styles */
.info-label {
  font-family: $font-family-base;
  font-weight: $font-weight-medium;
  font-size: $font-size-xs;
  line-height: 100%;
  color: $color-text-primary;
}

.info-value.warning {
  color: #aa2647;
}

.info-value {
  font-family: $font-family-base;
  font-weight: $font-weight-bold;
  font-size: $font-size-md;
  line-height: 20px;
  color: $color-text-primary;
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

</style>

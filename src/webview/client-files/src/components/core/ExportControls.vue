<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/core/ExportControls.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->
<template>
  <div class="export-controls">
    <button
      class="export-btn"
      @click="handleExport('json')"
      title="Export as JSON"
      type="button"
    >
      JSON data
    </button>
    <span class="separator">|</span>
    <button class="export-btn" @click="handleExport('csv')" title="Export as CSV" type="button">
      CSV data
    </button>
    <span class="separator">|</span>
    <button class="export-btn" @click="handleExport('svg')" title="Save as SVG" type="button">
      Save as SVG
    </button>
  </div>
</template>

<script setup lang="ts">
interface Props {
  chartId: string
}

const props = defineProps<Props>()

const emit = defineEmits<{
  export: [chartId: string, format: 'json' | 'csv' | 'svg']
}>()

const handleExport = (format: 'json' | 'csv' | 'svg') => {
  emit('export', props.chartId, format)
}
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.export-controls {
  display: flex;
  justify-content: center;
  align-items: center;
  gap: $spacing-sm;
  padding: $spacing-md 0;
  margin-bottom: $spacing-lg;
}

.export-btn {
  background: none;
  border: none;
  color: #2563eb;
  cursor: pointer;
  font-size: $font-size-sm;
  padding: $spacing-xs $spacing-sm;
  text-decoration: none;
  transition: all $transition-base;
}

.export-btn:hover {
  text-decoration: underline;
  color: #1d4ed8;
}

.export-btn:active {
  color: #1e40af;
}

.separator {
  color: #9ca3af;
  font-size: $font-size-sm;
}
</style>

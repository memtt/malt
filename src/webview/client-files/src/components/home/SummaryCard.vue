<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/home/SummaryCard.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->
<template>
  <div class="summary-card">
    <div v-for="(item, index) in props.items" :key="index" class="metric-item">
      <div class="metric-label">{{ item.title }}</div>
      <div class="metric-value">{{ item.value }}</div>
    </div>
  </div>
</template>

<script setup lang="ts">
type Props = {
  items: {
    title: string
    value: string | number
  }[]
}

const props = defineProps<Props>()
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.summary-card {
  display: flex;
  align-items: start;
  gap: $spacing-lg;
  padding: $spacing-xl $spacing-3xl;
  border: 1px solid #c2e3f5;
  border-radius: $radius-xl;
  background: $gradient-section;
  max-width: 600px;
  margin-bottom: $spacing-md;
}

.metric-item {
  display: flex;
  flex-direction: column;
  gap: $spacing-xs;
  width: 120px;
}

.metric-label {
  font-weight: $font-weight-medium;
  font-size: $font-size-xs;
  color: $color-text-primary;
}

.metric-value {
  font-weight: $font-weight-bold;
  font-size: $font-size-md;
  color: $color-text-primary;
}
</style>

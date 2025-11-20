<template>
  <div class="global-vars-summary">
    <div class="metric-item">
      <div class="metric-label">{{ props.label1 }}</div>
      <div class="metric-value">{{ props.value1 }}</div>
    </div>
    <div class="metric-item">
      <div class="metric-label">{{ props.label2 }}</div>
      <div class="metric-value">{{ props.value2 }}</div>
    </div>
    <div class="metric-item">
      <div class="metric-label">{{ props.label3 }}</div>
      <div class="metric-value">{{ props.value3 }}</div>
    </div>
  </div>
</template>

<script setup lang="ts">
type Props = {
  label1: string
  value1: string | number
  label2: string
  value2: string | number
  label3: string
  value3: string | number
}

const props = defineProps<Props>()
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.global-vars-summary {
  display: flex;
  align-items: center;
  justify-content: space-between;
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

<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/home/StatCard.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->
<template>
  <div class="stat-card" :class="{ 'has-warning': props.hasWarning }">
    <div class="stat-card-header">
      <component :is="props.icon" v-if="props.icon" class="stat-card-icon" />
      <h4 class="stat-card-title">{{ props.title }}</h4>
      <span v-if="props.hasWarning" class="warning-indicator" :title="props.warningMessage">⚠</span>
    </div>
    <div class="stat-card-content">
      <slot></slot>
    </div>
  </div>
</template>

<script setup lang="ts">
import type { Component } from 'vue';

interface Props {
  title: string;
  icon?: Component;
  hasWarning?: boolean;
  warningMessage?: string;
}

const props = withDefaults(defineProps<Props>(), {
  hasWarning: false,
  warningMessage: '',
});
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.stat-card {
  flex: 1;
  min-width: 280px;
  max-width: 365.33px;
  height: auto;
  min-height: 196px;
  padding: $spacing-lg;
  background: $color-bg-primary;
  border: 1px solid $color-border-gray;
  border-radius: $radius-lg;
  display: flex;
  flex-direction: column;
  gap: $spacing-lg;
  box-shadow: $shadow-sm;
  transition: all $transition-base $transition-ease;
}

.stat-card:hover {
  box-shadow: 0 4px 8px rgba(0, 0, 0, 0.08);
  transform: translateY(-1px);
}

.stat-card.has-warning {
  border-color: $color-warning;
  background: #fffbeb;
}

.stat-card-header {
  display: flex;
  align-items: center;
  gap: $spacing-sm;
  position: relative;
}

.stat-card-icon {
  width: 18px;
  height: 18px;
  color: $color-text-muted;
  flex-shrink: 0;
}

.stat-card-title {
  font-family: $font-family-base;
  font-weight: $font-weight-semibold;
  font-size: $font-size-sm;
  line-height: 18px;
  color: $color-text-dark;
  margin: 0;
  flex: 1;
}

.warning-indicator {
  color: $color-warning;
  font-size: $font-size-md;
  cursor: help;
}

.stat-card-content {
  flex: 1;
  display: flex;
  flex-direction: column;
  gap: $spacing-md;
}

.stat-card-content :deep(.stat-row) {
  display: flex;
  justify-content: space-between;
  align-items: baseline;
  gap: $spacing-md;
  padding: 6px 0;
  border-bottom: 1px solid $gray-100;
}

.stat-card-content :deep(.stat-row:last-child) {
  border-bottom: none;
}

.stat-card-content :deep(.stat-label) {
  font-size: 13px;
  font-weight: $font-weight-normal;
  color: $color-text-muted;
  flex: 1;
}

.stat-card-content :deep(.stat-value) {
  font-size: $font-size-base;
  font-weight: $font-weight-bold;
  color: $gray-900;
  text-align: right;
  font-family: $font-family-base;
}

.stat-card-content :deep(.stat-value.highlight) {
  color: $color-primary;
  font-size: $font-size-lg;
}

.stat-card-content :deep(.stat-value.warning) {
  color: $color-error;
}

@media (max-width: 768px) {
  .stat-card {
    min-width: 100%;
    max-width: 100%;
  }
}
</style>

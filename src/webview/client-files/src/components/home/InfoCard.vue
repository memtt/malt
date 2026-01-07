<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/home/InfoCard.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->
<template>
  <div class="info-card" :class="{ main: props.main }">
    <div class="card-header">
      <div class="header-left">
        <component :is="props.icon" v-if="props.icon" class="card-icon" />
        <h3 v-if="props.title" class="card-title">
          {{ props.title }}
        </h3>
      </div>
      <div class="header-right">
        <slot name="action"></slot>
        <button v-if="props.showHelp" class="help-link" @click="$emit('showHelp')">
          <IconInfo class="help-icon" />
          Show Help
        </button>
      </div>
    </div>
    <div class="card-body">
      <slot></slot>
    </div>
  </div>
</template>

<script setup lang="ts">
import type { Component, FunctionalComponent, SVGAttributes } from 'vue'
import IconInfo from '@/assets/icons/icon-info.svg?component'

interface Props {
  title?: string
  icon?: Component | FunctionalComponent<SVGAttributes>
  main?: boolean
  showHelp?: boolean
}

const props = defineProps<Props>()
defineEmits<{
  showHelp: []
}>()
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.info-card {
  display: flex;
  min-width: 280px;
  flex: 1;
  flex-direction: column;
  gap: $spacing-lg;
  border-radius: $radius-lg;
  border: 1px solid $color-border-light;
  background: $color-bg-primary;
  padding: $spacing-lg;
  transition: all $transition-base $transition-ease;
}

.info-card.main {
  border-color: #c2e3f5;
  background: linear-gradient(to bottom right, #e8f4f8, #d4e9f2);
}

.card-header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: $spacing-sm;
}

.header-left {
  display: flex;
  align-items: center;
  gap: $spacing-xs;
}

.header-right {
  display: flex;
  align-items: center;
  gap: $spacing-sm;
}

.card-title {
  margin: 0;
  display: flex;
  align-items: center;
  gap: 6px;
  font-size: $font-size-base;
  font-weight: $font-weight-normal;
  line-height: 1.25;
  color: $color-primary;
}

.card-body {
  display: flex;
  flex: 1;
  flex-direction: column;
  gap: 0;
  font-size: $font-size-sm;
  color: #2c3e50;
}

.help-link {
  display: flex;
  align-items: center;
  gap: 6px;
  background: transparent;
  border: none;
  padding: $spacing-xs $spacing-sm;
  font-family: $font-family-base;
  font-size: 13px;
  font-weight: $font-weight-medium;
  color: #3b82f6;
  cursor: pointer;
  border-radius: $radius-sm;
  transition: all $transition-base $transition-ease;
  white-space: nowrap;
}

.help-link:hover {
  background: rgba(59, 130, 246, 0.1);
  color: #1e40af;
}

.help-icon {
  width: 16px;
  height: 16px;
  flex-shrink: 0;
}

:deep(.info-row) {
  display: flex;
  flex-direction: column;
  gap: 2px;
}

:deep(.info-row + .info-row) {
  margin-top: $spacing-lg;
}

:deep(.info-command) {
  background: rgba(24, 72, 98, 0.05);
  padding: $spacing-sm;
  border-radius: $radius-sm;
  font-family: $font-family-mono;
  font-size: $font-size-xs;
  color: $color-primary;
  overflow-x: auto;
  white-space: pre-wrap;
  word-break: break-all;
}

@media (max-width: 768px) {
  .info-card {
    max-width: none;
    min-width: 0;
  }
}
</style>

<template>
  <div class="tree-row" :class="{ expanded: node.expanded }">
    <div class="node-name">
      <!-- Indentation -->
      <span class="indenter" :style="{ width: `${depth * 20}px` }" />

      <!-- Expand/Collapse icon -->
      <span
        v-if="node.hasChild"
        class="expander"
        :class="{ clickable: !node.loading }"
        @click.stop="onExpanderClick"
      >
        <span v-if="node.loading" class="loading-icon">⏳</span>
        <IconChevronDown v-else-if="node.expanded" class="chevron-icon" />
        <IconChevronRight v-else class="chevron-icon" />
      </span>
      <span v-else class="expander-placeholder" />

      <!-- Function name -->
      <span class="function-name" @click="onFunctionClick">
        {{ node.location.function }}
      </span>
    </div>

    <div class="node-value">
      {{ getFormattedValue(node) }}
    </div>
  </div>
</template>

<script setup lang="ts">
import IconChevronDown from '@/assets/icons/icon-chevron-down.svg?component'
import IconChevronRight from '@/assets/icons/icon-chevron-right.svg?component'
import type { CallStackNode } from '@/composables/useCallStackTree'

interface Props {
  node: CallStackNode
  depth: number
  getFormattedValue: (node: CallStackNode) => string
}

interface Emits {
  (e: 'toggle', node: CallStackNode): void
  (e: 'functionClick', node: CallStackNode): void
}

const props = defineProps<Props>()
const emit = defineEmits<Emits>()

/**
 * Handle click on expander icon (expand/collapse)
 */
const onExpanderClick = () => {
  if (props.node.hasChild && !props.node.loading) {
    emit('toggle', props.node)
  }
}

/**
 * Handle click on function name (open modal)
 */
const onFunctionClick = () => {
  emit('functionClick', props.node)
}
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.tree-row {
  display: flex;
  align-items: center;
  transition: background-color $transition-fast;
  border-radius: $radius-md;
  overflow: hidden;
  background: $color-bg-primary;
  border: 1px solid $color-border-light;
}

.tree-row.expanded {
  background-color: #fafafa;
}

.node-name {
  flex: 1;
  padding: 0.5rem 1rem;
  display: flex;
  align-items: center;
  font-family: $font-family-mono;
  font-size: 0.813rem;
  user-select: none;
}

.indenter {
  display: inline-block;
  flex-shrink: 0;
  position: relative;
}

.indenter::before {
  content: '';
  position: absolute;
  left: 10px;
  top: 50%;
  width: 1px;
  height: 100%;
  background: $color-border-gray;
}

.expander {
  display: inline-flex;
  align-items: center;
  justify-content: center;
  width: 16px;
  height: 16px;
  flex-shrink: 0;
  transition: opacity $transition-fast;
}

.expander.clickable {
  cursor: pointer;
}

.expander.clickable:hover {
  opacity: 0.7;
}

.chevron-icon {
  width: 12px;
  height: 12px;
}

.chevron-icon :deep(path) {
  fill: $color-primary;
}

.loading-icon {
  font-size: 0.75rem;
}

.expander-placeholder {
  display: inline-block;
  width: 16px;
  flex-shrink: 0;
}

.function-name {
  color: $color-text-primary;
  margin-left: $spacing-xs;
  font-weight: $font-weight-normal;
  cursor: pointer;
  transition: color $transition-fast;
}

.function-name:hover {
  color: $color-primary-light;
  text-decoration: underline;
}

.node-value {
  width: 150px;
  padding: 0.5rem 1rem;
  text-align: right;
  font-weight: $font-weight-semibold;
  font-size: $font-size-sm;
  color: $gray-900;
  flex-shrink: 0;
}
</style>

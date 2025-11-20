<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/call-tree/CallStackTree.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->

<template>
  <div class="call-stack-tree">
    <!-- Loading state -->
    <div v-if="loading && nodes.length === 0" class="tree-loading">
      <div class="spinner-border" role="status">
        <span class="visually-hidden">Loading...</span>
      </div>
      <p class="loading-text">Loading call stacks...</p>
    </div>

    <!-- Error state -->
    <div v-else-if="error" class="tree-error">
      <p>{{ error }}</p>
    </div>

    <!-- Tree container -->
    <div v-else class="tree-container">
      <!-- Header -->
      <div class="tree-header">
        <div class="header-function">Function</div>
        <div class="header-value">{{ metricName }}</div>
      </div>

      <!-- Tree rows -->
      <div class="tree-rows">
        <CallStackTreeRow
          v-for="node in nodes"
          :key="`${node.stackId}-${node.stackDepth}`"
          :node="node"
          :depth="node.depth"
          :get-formatted-value="getFormattedValue"
          @toggle="onToggle"
          @function-click="onFunctionClick"
        />
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import CallStackTreeRow from '@/components/call-tree/CallStackTreeRow.vue'
import type { CallStackNode } from '@/composables/useCallStackTree'

interface Props {
  nodes: CallStackNode[]
  loading?: boolean
  error?: string | null
  metricName: string
  getFormattedValue: (node: CallStackNode) => string
}

interface Emits {
  (e: 'expand', node: CallStackNode): void
  (e: 'collapse', node: CallStackNode): void
  (e: 'functionClick', node: CallStackNode): void
}

withDefaults(defineProps<Props>(), {
  loading: false,
  error: null,
})

const emit = defineEmits<Emits>()

/**
 * Toggle node expansion
 */
const onToggle = (node: CallStackNode) => {
  if (node.expanded) {
    emit('collapse', node)
  } else {
    emit('expand', node)
  }
}

/**
 * Handle function name click
 */
const onFunctionClick = (node: CallStackNode) => {
  emit('functionClick', node)
}
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.call-stack-tree {
  height: 100%;
  display: flex;
  flex-direction: column;
}

.tree-loading,
.tree-error {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  padding: 2rem;
  min-height: 200px;
  flex: 1;
}

.tree-error {
  color: $color-error;
}

.tree-container {
  flex: 1;
  display: flex;
  flex-direction: column;
  overflow: hidden;
}

/* Header */
.tree-header {
  display: flex;
  align-items: center;
  position: sticky;
  top: 0;
  z-index: 10;
  background: $color-bg-lighter;
  border-radius: $radius-lg;
  padding: 0.75rem 1rem;
  margin-bottom: $spacing-sm;
}

.header-function {
  flex: 1;
  font-weight: $font-weight-medium;
  font-size: $font-size-xs;
  color: #000;
}

.header-value {
  width: 150px;
  text-align: right;
  font-weight: $font-weight-medium;
  font-size: $font-size-xs;
  color: #000;
}

/* Rows container */
.tree-rows {
  flex: 1;
  overflow: auto;
  display: flex;
  flex-direction: column;
  gap: $spacing-xs;
}

/* Spinner */
.spinner-border {
  width: 2rem;
  height: 2rem;
  border: 0.25em solid $color-primary;
  border-right-color: transparent;
  border-radius: 50%;
  animation: spinner-border 0.75s linear infinite;
}

@keyframes spinner-border {
  to {
    transform: rotate(360deg);
  }
}

.visually-hidden {
  position: absolute;
  width: 1px;
  height: 1px;
  padding: 0;
  margin: -1px;
  overflow: hidden;
  clip: rect(0, 0, 0, 0);
  white-space: nowrap;
  border: 0;
}

.loading-text {
  margin-top: $spacing-sm;
}
</style>

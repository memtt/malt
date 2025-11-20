<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/stack-peaks/StackPeaksSidebar.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->
<template>
  <div class="stack-peaks-sidebar">
    <!-- Global Stack Option -->
    <div class="thread-item">
      <a
        href="#"
        class="thread-link"
        :class="{ active: selectedThreadId === null }"
        @click.prevent="emit('threadSelect', null)"
      >
        Global Stack
      </a>
    </div>

    <!-- Thread List -->
    <div v-for="(thread, index) in threads" :key="index" class="thread-item">
      <a
        href="#"
        class="thread-link"
        :class="{ active: selectedThreadId === index }"
        @click.prevent="emit('threadSelect', index)"
      >
        Thread {{ index }}
      </a>
      <div class="thread-size">{{ formatSize(thread.size) }}</div>
    </div>
  </div>
</template>

<script setup lang="ts">
import type { StackInfo } from '@/types/stack-peaks'
import { humanReadable } from '@/lib/helpers'

interface Props {
  threads: StackInfo[]
  selectedThreadId: number | null
  largestStack: number
}

interface Emits {
  (e: 'threadSelect', threadId: number | null): void
}

const props = defineProps<Props>()
const emit = defineEmits<Emits>()

/**
 * Format size value
 */
const formatSize = (bytes: number): string => {
  return humanReadable(bytes, 1, 'B', true)
}
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.stack-peaks-sidebar {
  display: flex;
  flex-direction: column;
  flex-shrink: 0;
  align-self: flex-start;
  position: sticky;
  top: 20px;
  background: $color-bg-primary;
  border-radius: $radius-xl;
  padding: $spacing-3xl;
  width: 228px;
  gap: $spacing-md;
  max-height: calc(100vh - 300px);
  min-height: 400px;
  overflow: auto;
}

.thread-item:last-child {
  margin-bottom: 0;
}

.thread-link {
  display: block;
  font-size: $font-size-base;
  font-weight: $font-weight-normal;
  color: $color-primary-light;
  text-decoration: none;
  transition: color $transition-base ease;
}

.thread-link:hover {
  text-decoration: underline;
}

.thread-link.active {
  font-weight: $font-weight-bold;
  color: $color-text-primary;
  text-decoration: none;
}

.thread-link.active:hover {
  color: $color-text-primary;
  text-decoration: none;
}

.thread-size {
  font-size: $font-size-xs;
  font-weight: $font-weight-medium;
  color: $color-text-primary;
  margin-top: $spacing-xs;
}
</style>

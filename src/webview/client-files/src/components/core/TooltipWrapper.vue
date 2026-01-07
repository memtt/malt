<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/core/TooltipWrapper.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->
<template>
  <div class="tooltip-wrapper" @mouseenter="showTooltip" @mouseleave="hideTooltip">
    <slot></slot>
    <Teleport to="body">
      <div v-if="isVisible" ref="tooltipRef" class="tooltip" :style="tooltipStyle">
        {{ content }}
      </div>
    </Teleport>
  </div>
</template>

<script setup lang="ts">
import { ref, computed, onMounted, onBeforeUnmount } from 'vue'

interface Props {
  content: string
}

const props = defineProps<Props>()

const isVisible = ref(false)
const tooltipRef = ref<HTMLElement | null>(null)
const mouseX = ref(0)
const mouseY = ref(0)

const showTooltip = (event: MouseEvent) => {
  isVisible.value = true
  mouseX.value = event.clientX
  mouseY.value = event.clientY
}

const hideTooltip = () => {
  isVisible.value = false
}

const handleScroll = () => {
  if (isVisible.value) {
    hideTooltip()
  }
}

const tooltipStyle = computed(() => ({
  left: `${mouseX.value}px`,
  top: `${mouseY.value + 10}px`,
}))

onMounted(() => {
  window.addEventListener('scroll', handleScroll, true)
})

onBeforeUnmount(() => {
  window.removeEventListener('scroll', handleScroll, true)
})
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.tooltip-wrapper {
  display: block;
  position: relative;
  width: 100%;
  min-width: 0;
  overflow: hidden;
}

.tooltip {
  position: fixed;
  z-index: 9999;
  max-width: 592px;
  height: auto;
  min-height: 34px;
  border-radius: $radius-md;
  padding: $spacing-sm $spacing-md;
  background: $color-primary;
  color: white;
  font-family: $font-family-base;
  font-size: $font-size-sm;
  line-height: 1.4;
  word-break: break-word;
  pointer-events: none;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.15);
  transform: translateX(-50%);
}
</style>

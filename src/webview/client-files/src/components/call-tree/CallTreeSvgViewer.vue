<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/call-tree/CallTreeSvgViewer.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
|    AUTHOR   : Sébastien Valat (INRIA) - 2025
----------------------------------------------------------->

<template>
  <div class="call-tree-svg-viewer">
    <!-- Loading State -->
    <div v-if="loading" class="viewer-state loading">
      <div class="spinner-border" role="status">
        <span class="visually-hidden">Loading...</span>
      </div>
      <p class="loading-text">Loading call tree graph...</p>
    </div>

    <!-- Error States -->
    <div v-else-if="error || data?.error" class="viewer-state">
      <!-- SVG Generation Error -->
      <div v-if="data?.error?.svgGenerationError" class="error-message">
        <span class="error-icon">⊗</span>
        <p>{{ data.error.svgGenerationError }}</p>
      </div>

      <!-- Filter Error -->
      <div v-else-if="data?.error?.filterError" class="error-message">
        <span class="error-icon">👁</span>
        <p>{{ data.error.filterError }}</p>
      </div>

      <!-- Network Error -->
      <div v-else-if="data?.error?.networkError" class="error-message">
        <span class="error-icon">⚠</span>
        <p>{{ data.error.networkError }}</p>
      </div>

      <!-- Generic Error -->
      <div v-else-if="error" class="error-message">
        <span class="error-icon">⚠</span>
        <p>{{ error }}</p>
      </div>
    </div>

    <!-- SVG Container -->
    <div v-else ref="svgContainerRef" class="svg-container">
      <!-- Context Menu (General) -->
      <div
        v-if="showGeneralContextMenu"
        ref="generalContextMenuRef"
        class="context-menu"
        :style="contextMenuStyle"
        @click.stop
        @mousedown.stop
      >
        <ul>
          <li @mousedown.prevent.stop="fitGraph">
            <span class="menu-icon">⛶</span>
            Resize to Fit
          </li>
          <li class="divider"></li>
          <li @mousedown.prevent.stop="downloadSvg">
            <span class="menu-icon">🖼</span>
            Download as SVG
          </li>
          <li @mousedown.prevent.stop="downloadDot">
            <span class="menu-icon">⬇</span>
            Download as Dot
          </li>
        </ul>
      </div>

      <!-- Context Menu (Node) -->
      <div
        v-if="showNodeContextMenu"
        ref="nodeContextMenuRef"
        class="context-menu"
        :style="contextMenuStyle"
        @click.stop
        @mousedown.stop
      >
        <ul>
          <li @mousedown.prevent.stop="selectNode">
            <span class="menu-icon">🎯</span>
            Select Function
          </li>
          <li @mousedown.prevent.stop="viewFunctionSource">
            <span class="menu-icon">📝</span>
            View Function Source
          </li>
          <li class="divider"></li>
          <li @mousedown.prevent.stop="fitGraph">
            <span class="menu-icon">⛶</span>
            Resize to Fit
          </li>
          <li @mousedown.prevent.stop="downloadSvg">
            <span class="menu-icon">🖼</span>
            Download as SVG
          </li>
          <li @mousedown.prevent.stop="downloadDot">
            <span class="menu-icon">⬇</span>
            Download as Dot
          </li>
        </ul>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, watch, onMounted, onBeforeUnmount, nextTick } from 'vue'
import { useRouter } from 'vue-router'
import svgPanZoom from 'svg-pan-zoom'
import { downloadCallTreeFile } from '@/services/callTreeService'
import type { CallTreeData, CallTreeParams } from '@/types/call-tree'

type SvgPanZoomInstance = ReturnType<typeof svgPanZoom>

interface Props {
  data?: CallTreeData
  loading?: boolean
  error?: string | null
  metric?: string
  ratio?: boolean
  depth?: number
  height?: number
  nodeCost?: number
  currentNodeId?: number
}

interface Emits {
  (e: 'node-select', nodeId: number): void
}

const props = withDefaults(defineProps<Props>(), {
  metric: 'peakmem.global',
  ratio: false,
  depth: 3,
  height: -1,
  nodeCost: 1,
  currentNodeId: -1,
})
const emit = defineEmits<Emits>()
const router = useRouter()

// Refs
const svgContainerRef = ref<HTMLDivElement | null>(null)
const generalContextMenuRef = ref<HTMLDivElement | null>(null)
const nodeContextMenuRef = ref<HTMLDivElement | null>(null)

// State
const panZoomInstance = ref<SvgPanZoomInstance | null>(null)
const showGeneralContextMenu = ref(false)
const showNodeContextMenu = ref(false)
const contextMenuStyle = ref({ left: '0px', top: '0px' })
const contextMenuNodeId = ref<number>(-1)
const contextMenuFunction = ref<string>('')

/**
 * Parse SVG string and insert into DOM
 */
const parseSvg = (svgString: string): SVGElement | null => {
  const parser = new DOMParser()
  const doc = parser.parseFromString(svgString, 'image/svg+xml')
  const svgElement = doc.documentElement as unknown as SVGElement

  if (svgElement.tagName === 'svg') {
    svgElement.id = 'svggraph'
    return svgElement
  }

  return null
}

/**
 * Initialize SVG pan-zoom
 */
const initializePanZoom = () => {
  if (!svgContainerRef.value) return

  // Remove existing SVG if any
  const existingSvg = svgContainerRef.value.querySelector('#svggraph')
  if (existingSvg) {
    existingSvg.remove()
  }

  // Destroy existing pan-zoom instance
  if (panZoomInstance.value) {
    panZoomInstance.value.destroy()
    panZoomInstance.value = null
  }

  // Parse and insert new SVG
  if (props.data?.svg) {
    const svgElement = parseSvg(props.data.svg)
    if (svgElement) {
      svgContainerRef.value.appendChild(svgElement)

      // Initialize svg-pan-zoom
      nextTick(() => {
        try {
          panZoomInstance.value = svgPanZoom('#svggraph', {
            zoomEnabled: true,
            controlIconsEnabled: false,
            fit: true,
            center: true,
            dblClickZoomEnabled: false,
            maxZoom: 1000,
          })

          // Attach event listeners to nodes
          attachNodeEventListeners()
        } catch (err) {
          console.error('Failed to initialize svg-pan-zoom:', err)
        }
      })
    }
  }
}

/**
 * Attach event listeners to SVG nodes
 */
const attachNodeEventListeners = () => {
  if (!svgContainerRef.value) return

  const nodes = svgContainerRef.value.querySelectorAll('.node')

  nodes.forEach((node) => {
    // Double-click: Select node
    node.addEventListener('dblclick', (e) => {
      e.preventDefault()
      const title = node.querySelector('title')
      if (title) {
        const nodeId = parseInt(title.textContent?.replace('node', '') || '-1')
        emit('node-select', nodeId)
      }
    })

    // Right-click: Show context menu
    node.addEventListener('contextmenu', (e) => {
      e.preventDefault()
      const title = node.querySelector('title')
      const anchor = node.querySelector('a')

      if (title) {
        contextMenuNodeId.value = parseInt(title.textContent?.replace('node', '') || '-1')
        contextMenuFunction.value = anchor?.getAttribute('xlink:title') || ''

        showGeneralContextMenu.value = false
        showNodeContextMenu.value = true

        contextMenuStyle.value = {
          left: `${(e as MouseEvent).pageX}px`,
          top: `${(e as MouseEvent).pageY}px`,
        }
      }
    })

    // Prevent default anchor behavior
    const anchor = node.querySelector('a')
    if (anchor) {
      anchor.addEventListener('click', (e) => {
        e.preventDefault()
      })
    }
  })

  // General context menu (on SVG container)
  svgContainerRef.value.addEventListener('contextmenu', (e) => {
    // Check if click is on node
    const target = e.target as HTMLElement
    if (target.closest('.node')) return

    e.preventDefault()
    showNodeContextMenu.value = false
    showGeneralContextMenu.value = true

    contextMenuStyle.value = {
      left: `${e.pageX}px`,
      top: `${e.pageY}px`,
    }
  })
}

/**
 * Hide context menus on outside click
 */
const hideContextMenus = () => {
  showGeneralContextMenu.value = false
  showNodeContextMenu.value = false
}

/**
 * Fit graph to container
 */
const fitGraph = () => {
  if (panZoomInstance.value) {
    panZoomInstance.value.reset()
    panZoomInstance.value.fit()
    panZoomInstance.value.center()
  }
  hideContextMenus()
}

/**
 * Select node (navigate to it)
 */
const selectNode = () => {
  emit('node-select', contextMenuNodeId.value)
  hideContextMenus()
}

/**
 * View function source
 */
const viewFunctionSource = () => {
  if (contextMenuFunction.value) {
    router.push({
      path: '/sources',
      query: {
        func: contextMenuFunction.value,
        metric: props.metric,
      },
    })
  }
  hideContextMenus()
}

/**
 * Download SVG
 */
const downloadSvg = async () => {
  const params: CallTreeParams = {
    nodeid: props.currentNodeId,
    func: null,
    depth: props.depth,
    height: props.height,
    mincost: props.nodeCost,
    metric: props.metric,
    isratio: props.ratio,
  }

  try {
    await downloadCallTreeFile(params, 'svg', 'calltree.svg')
  } catch (err) {
    console.error('Failed to download SVG:', err)
    alert('Failed to download SVG file. Please try again.')
  }

  hideContextMenus()
}

/**
 * Download DOT
 */
const downloadDot = async () => {
  const params: CallTreeParams = {
    nodeid: props.currentNodeId,
    func: null,
    depth: props.depth,
    height: props.height,
    mincost: props.nodeCost,
    metric: props.metric,
    isratio: props.ratio,
  }

  try {
    await downloadCallTreeFile(params, 'dot', 'calltree.dot')
  } catch (err) {
    console.error('Failed to download DOT:', err)
    alert('Failed to download DOT file. Please try again.')
  }

  hideContextMenus()
}

/**
 * Watch for data changes
 */
watch(
  () => props.data,
  () => {
    if (props.data && !props.loading && !props.error) {
      nextTick(() => {
        initializePanZoom()
      })
    }
  },
  { immediate: true },
)

/**
 * Mount: Add global click listener to hide context menus
 */
onMounted(() => {
  // Use setTimeout to delay the click listener registration
  // This prevents immediate closure when opening the menu
  setTimeout(() => {
    document.addEventListener('click', hideContextMenus)
  }, 0)
})

/**
 * Unmount: Cleanup
 */
onBeforeUnmount(() => {
  document.removeEventListener('click', hideContextMenus)
  if (panZoomInstance.value) {
    panZoomInstance.value.destroy()
  }
})

/**
 * Expose fitGraph method to parent
 */
defineExpose({
  fitGraph,
})
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.call-tree-svg-viewer {
  width: 100%;
  height: 100%;
  position: relative;
  background: $color-bg-primary;
  overflow: hidden;
}

/* States */
.viewer-state {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  height: 100%;
  min-height: 500px;

  &.loading {
    background: $gray-50;
  }

  p {
    margin: 0;
    color: $gray-500;
    font-size: $font-size-md;
  }
}

.loading-text {
  margin-top: $spacing-md;
}

/* Error message */
.error-message {
  text-align: center;
  color: $color-error;
  font-size: $font-size-lg;
}

.error-icon {
  display: block;
  font-size: $font-size-4xl;
  margin-bottom: $spacing-lg;
  opacity: 0.5;
}

/* SVG Container */
.svg-container {
  width: 100%;
  height: 100%;
  min-height: 500px;
  max-height: 600px;
  position: relative;
}

:deep(#svggraph) {
  width: 100%;
  height: 100%;
  cursor: grab;

  &:active {
    cursor: grabbing;
  }

  a:hover {
    text-decoration: none;
  }
}

:deep(.node) {
  cursor: pointer;
}

/* Context Menu */
.context-menu {
  position: fixed;
  z-index: 1000;
  background: $color-bg-primary;
  border: 1px solid $gray-300;
  border-radius: $radius-sm;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.15);
  min-width: 200px;

  ul {
    list-style: none;
    margin: 0;
    padding: 0.25rem 0;
  }

  li {
    padding: $spacing-sm $spacing-lg;
    cursor: pointer;
    display: flex;
    align-items: center;
    gap: $spacing-sm;
    font-size: $font-size-sm;
    transition: background-color $transition-fast ease;

    &:hover {
      background-color: $gray-50;
    }

    &.divider {
      height: 1px;
      background: $gray-300;
      padding: 0;
      margin: 0.25rem 0;
      cursor: default;

      &:hover {
        background: $gray-300;
      }
    }
  }
}

.menu-icon {
  font-size: $font-size-md;
  opacity: 0.7;
}

/* Spinner */
.spinner-border {
  width: 3rem;
  height: 3rem;
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
</style>

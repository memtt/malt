<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/views/SourcesPage.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->
<template>
  <div class="sources-page">
    <!-- Loading State -->
    <div v-if="loading" class="loading-state">
      <div class="spinner-border" role="status">
        <span class="visually-hidden">Loading...</span>
      </div>
      <p class="loading-text">Loading source data...</p>
    </div>

    <!-- Error State -->
    <div v-else-if="error" class="alert alert-danger" role="alert">
      <h4 class="alert-heading">Error loading data</h4>
      <p>{{ error }}</p>
    </div>

    <!-- Main Content -->
    <div v-else class="sources-layout">
      <!-- Sidebar -->
      <aside class="sources-sidebar-container">
        <SourcesSidebar
          :selector="selector"
          :all-functions="functions"
          :selected-function="editor.currentFunction.value"
          @function-select="onFunctionSelect"
        />
      </aside>

      <!-- Main Content Area -->
      <main class="sources-main">
        <!-- Warning: No source available -->
        <div v-if="!editor.hasSource.value" class="no-source-warning" role="alert">
          <h3 class="warning-title">No Source Code Available</h3>
          <p class="warning-message">
            No source code available for function
            <strong>{{ editor.currentFunction.value }}</strong>
            <template v-if="editor.currentFile.value && editor.currentFile.value !== '??' && editor.currentFile.value !== ''">
              <br />
              in file <em class="file-name">{{ editor.currentFile.value }}</em>
            </template>
          </p>
        </div>

        <!-- Source Editor -->
        <div v-else class="source-editor-section">
          <!-- File header -->
          <FileHeader
            :file-path="editor.currentFile.value"
            :function-name="editor.currentFunction.value"
          />

          <!-- Code Editor -->
          <SourceCodeEditor
            ref="sourceCodeEditorRef"
            :source-code="editor.sourceCode.value"
            :current-file="editor.currentFile.value"
            :annotations="editor.annotations.value"
            :loading="editor.loading.value"
            :error="editor.error.value"
            :selected-line="editor.selectedLine.value"
            :get-annotation-value="getAnnotationValue"
            :get-formatted-value="getFormattedAnnotationValue"
            @annotation-click="onAnnotationClick"
          />
        </div>

        <!-- Bottom Section: Call Stack Tree (full width) -->
        <div class="bottom-section">
          <!-- Call Stack Tree -->
          <div class="call-stack-container">
            <CallStackTree
              :nodes="callStackTree.flatNodes.value"
              :loading="callStackTree.loading.value"
              :error="callStackTree.error.value"
              :metric-name="selector.currentMetricName.value"
              :get-formatted-value="getCallStackNodeFormattedValue"
              :get-value="getCallStackNodeValue"
              @expand="callStackTree.expandNode"
              @collapse="callStackTree.collapseNode"
              @function-click="onCallStackFunctionClick"
            />
          </div>
        </div>
      </main>
    </div>

    <!-- Line Metrics Modal -->
    <LineMetricsModal
      v-model="isMetricsModalOpen"
      :annotation="selectedAnnotation"
      @point-to-call-stack="onPointToCallStack"
    />
  </div>
</template>

<script setup lang="ts">
import { ref, watch } from 'vue'
import { useRoute } from 'vue-router'
import { useSources } from '@/composables/useSources'
import { useSourceEditor } from '@/composables/useSourceEditor'
import { useCallStackTree } from '@/composables/useCallStackTree'
import SourcesSidebar from '@/components/sources/SourcesSidebar.vue'
import SourceCodeEditor from '@/components/sources/SourceCodeEditor.vue'
import LineMetricsModal from '@/components/sources/LineMetricsModal.vue'
import CallStackTree from '@/components/call-tree/CallStackTree.vue'
import FileHeader from '@/components/shared/FileHeader.vue'
import type { FunctionStat, SourceAnnotation } from '@/types/sources'
import type { CallStackNode } from '@/composables/useCallStackTree'

const route = useRoute()

// Composables
const { loading, error, selector, functions } = useSources({
  initialMetric: (route.query.metric as any) || 'peakmem.global',
})

/**
 * Get annotation value for color gradient
 */
const getAnnotationValue = (annotation: SourceAnnotation): number => {
  return selector.getValue({
    function: annotation.function || '',
    total: annotation.total!,
    own: annotation.own,
  } as FunctionStat)
}

const editor = useSourceEditor({
  getValue: getAnnotationValue,
})

const callStackTree = useCallStackTree()

// Refs
const sourceCodeEditorRef = ref<InstanceType<typeof SourceCodeEditor> | null>(null)
const isMetricsModalOpen = ref(false)
const selectedAnnotation = ref<SourceAnnotation | null>(null)

/**
 * Handle function selection from sidebar
 */
const onFunctionSelect = async (func: FunctionStat) => {
  selectedAnnotation.value = null

  await editor.moveToFileFunction(func.file || '??', func.function)

  // Update call stack tree
  if (func.function) {
    await callStackTree.updateByFunction(func.function)
  }

  // Set selected annotation to function stats
  selectedAnnotation.value = {
    line: func.line || 0,
    file: func.file,
    function: func.function,
    total: func.total,
    own: func.own,
  }
}

/**
 * Handle annotation click in code editor
 */
const onAnnotationClick = async (annotation: SourceAnnotation) => {
  selectedAnnotation.value = annotation
  isMetricsModalOpen.value = true

  // Update call stack tree with file + line
  if (annotation.file && annotation.line) {
    await callStackTree.updateByLocation(annotation.file, annotation.line)
  }
}

/**
 * Handle point to call stack - scroll to the selected annotation line
 */
const onPointToCallStack = () => {
  if (selectedAnnotation.value?.line) {
    sourceCodeEditorRef.value?.scrollToLine(selectedAnnotation.value.line)
  }
}

/**
 * Handle function name click from call stack tree - open metrics modal
 */
const onCallStackFunctionClick = (node: CallStackNode) => {
  // Convert CallStackNode to SourceAnnotation
  selectedAnnotation.value = {
    line: node.location.line || 0,
    file: node.location.file,
    function: node.location.function,
    total: node.infos,
    own: node.infos,
  }
  isMetricsModalOpen.value = true
}

/**
 * Get formatted annotation value
 */
const getFormattedAnnotationValue = (annotation: SourceAnnotation): string => {
  return selector.getFormattedValue({
    function: annotation.function || '',
    total: annotation.total!,
    own: annotation.own,
  } as FunctionStat)
}

/**
 * Get formatted call stack node value
 */
const getCallStackNodeFormattedValue = (node: CallStackNode): string => {
  return selector.getFormattedValue({
    function: node.location.function,
    total: node.infos,
    own: node.infos,
  } as FunctionStat)
}

/**
 * Get call stack node value
 */
const getCallStackNodeValue = (node: CallStackNode): number => {
  return selector.getValue({
    function: node.location.function,
    total: node.infos,
    own: node.infos,
  } as FunctionStat)
}

/**
 * Handle metric change - redraw annotations
 */
watch(
  [() => selector.metric.value, () => selector.inclusive.value, () => selector.ratio.value],
  () => {
    // Redraw annotations in editor
    sourceCodeEditorRef.value?.drawAnnotations()

    // Update call stack tree if function is selected
    if (editor.currentFunction.value) {
      callStackTree.updateByFunction(editor.currentFunction.value)
    }
  },
)

/**
 * Initialize from route params
 * Wait for functions to be loaded before selecting
 */
watch(
  () => functions.value.length,
  async (length) => {
    if (length > 0) {
      // Handle ?func= query param
      if (route.query.func) {
        const funcName = route.query.func as string
        const func = functions.value.find((f) => f.function === funcName)
        if (func) {
          onFunctionSelect(func)
        }
      }
      // Handle ?file=&line= query params (from global vars)
      else if (route.query.file && route.query.line) {
        const file = route.query.file as string
        const line = parseInt(route.query.line as string, 10)

        if (!isNaN(line)) {
          await editor.moveToFileLine(file, line)
        }
      }
    }
  },
  { immediate: true },
)
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.sources-page {
  display: flex;
  flex-direction: column;
  border: 1px solid $color-border-light;
  border-radius: $radius-xl;
  background: $blue-section-bg;
}

.loading-state {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  height: 100vh;
  background: $blue-section-bg;
  border-radius: $radius-xl;
}

.sources-layout {
  display: flex;
  flex: 1;
  overflow: hidden;
  padding: $spacing-3xl;
  gap: $spacing-3xl;
}

.sources-sidebar-container {
  width: 320px;
  flex-shrink: 0;
  overflow: hidden;
}

.sources-main {
  flex: 1;
  display: flex;
  flex-direction: column;
  overflow: hidden;
}

.no-source-warning {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  background: #fff5f5;
  border: 2px solid #ff6b6b;
  border-radius: $radius-md;
  padding: 2rem;
  margin-bottom: $spacing-lg;
  text-align: center;
  min-height: 150px;
  max-height: 250px;
}

.warning-title {
  color: $color-error;
  font-size: 1.25rem;
  font-weight: $font-weight-semibold;
  margin: 0 0 0.75rem 0;
}

.warning-message {
  color: #721c24;
  font-size: 1rem;
  margin: 0;
  line-height: 1.5;
}

.warning-message strong {
  color: $color-error;
  font-weight: $font-weight-semibold;
}

.warning-message .file-name {
  color: #666;
  font-style: italic;
  font-weight: normal;
}

.source-editor-section {
  flex: 1;
  display: flex;
  flex-direction: column;
  overflow: hidden;
  background: white;
  margin-bottom: $spacing-lg;
}

.bottom-section {
  display: flex;
}

.call-stack-container {
  width: 100%;
  overflow: hidden;
  min-height: 300px;
}

/* Responsive */
@media (max-width: 1200px) {
  .sources-sidebar-container {
    width: 300px;
  }

  .bottom-section {
    height: auto;
    min-height: 310px;
  }
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

.loading-text {
  margin-top: $spacing-md;
  font-family: $font-family-base;
  color: $gray-700;
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

/* Alert */
.alert {
  padding: 1rem;
  margin: $spacing-md;
  border: 1px solid transparent;
  border-radius: $spacing-xs;
}

.alert-danger {
  color: #721c24;
  background-color: $color-error-bg;
  border-color: $color-error-border;
}

.alert-heading {
  margin-top: 0;
  margin-bottom: 0.5rem;
}
</style>

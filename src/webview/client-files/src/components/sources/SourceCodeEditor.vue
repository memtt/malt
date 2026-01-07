<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/sources/SourceCodeEditor.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->
<template>
  <div class="source-code-editor">
    <!-- Loading state -->
    <div v-if="loading" class="editor-loading">
      <div class="spinner-container">
        <div class="spinner"></div>
        <p class="loading-text">Loading source file...</p>
      </div>
    </div>

    <!-- Error state -->
    <div v-else-if="error" class="editor-error">
      <p>{{ error }}</p>
    </div>

    <!-- Editor content -->
    <div v-else ref="editorContainer" class="editor-container">
      <pre
        :class="['line-numbers', languageClass]"
      ><code ref="codeElement">{{ sourceCode }}</code></pre>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, watch, onMounted, onBeforeUnmount, nextTick, computed } from 'vue'
import Prism from '@/lib/prism/init' // Centralized Prism initialization
import {
  addAnnotation,
  removeAllAnnotations,
  scrollToAndHighlight,
} from '@/lib/prism/codeAnnotator'
import { detectLanguage, getLanguageClass } from '@/utils/sources'
import type { SourceAnnotation } from '@/types/sources'

interface Props {
  sourceCode: string
  currentFile: string | null
  annotations: SourceAnnotation[]
  loading?: boolean
  error?: string | null
  selectedLine?: number | null
  getAnnotationValue: (annotation: SourceAnnotation) => number
  getFormattedValue: (annotation: SourceAnnotation) => string
}

interface Emits {
  (e: 'annotationClick', annotation: SourceAnnotation): void
  (e: 'lineClick', line: number): void
}

const props = withDefaults(defineProps<Props>(), {
  loading: false,
  error: null,
  selectedLine: null,
})

const emit = defineEmits<Emits>()

const editorContainer = ref<HTMLElement | null>(null)
const codeElement = ref<HTMLElement | null>(null)

// Detect language for syntax highlighting
const languageClass = computed(() => {
  if (!props.currentFile) return 'language-cpp'
  const language = detectLanguage(props.currentFile)
  return getLanguageClass(language)
})

/**
 * Highlight code with Prism
 */
const highlightCode = () => {
  nextTick(() => {
    if (!codeElement.value) {
      console.warn('SourceCodeEditor: codeElement not found')
      return
    }

    if (!editorContainer.value) {
      console.warn('SourceCodeEditor: editorContainer not found')
      return
    }

    const preElement = editorContainer.value.querySelector('pre')
    if (!preElement) {
      console.warn('SourceCodeEditor: pre element not found')
      return
    }

    // Force Prism to re-highlight the code element
    Prism.highlightElement(codeElement.value)

    // The line-numbers plugin automatically initializes via Prism's 'complete' hook
    // But we need to ensure it runs after highlighting
    nextTick(() => {
      // Check if line numbers were added
      const lineNumbersRows = preElement.querySelector('.line-numbers-rows')

      if (!lineNumbersRows) {
        console.warn('[SourceCodeEditor] Line numbers not initialized, manually triggering')
        // Manually trigger line numbers if the plugin didn't auto-init
        const plugins = Prism.plugins as unknown as Record<string, unknown>
        if (plugins?.lineNumbers) {
          const lineNumbers = plugins.lineNumbers as { init?: () => void }
          if (typeof lineNumbers.init === 'function') {
            lineNumbers.init()
          }
        }
      }

      // Draw annotations after a short delay to ensure Prism is done
      setTimeout(() => {
        drawAnnotations()
      }, 50)
    })
  })
}

/**
 * Draw annotations on code
 */
const drawAnnotations = () => {
  if (!codeElement.value) return

  // Clear existing annotations
  removeAllAnnotations(codeElement.value)

  // Calculate max value for color gradient
  const maxValue = Math.max(...props.annotations.map((ann) => props.getAnnotationValue(ann)), 1)

  // Add annotations
  props.annotations.forEach((annotation) => {
    const value = props.getAnnotationValue(annotation)
    if (value === 0) return

    const color = getColorForValue(value, maxValue)

    addAnnotation(codeElement.value!, {
      line: annotation.line,
      text: props.getFormattedValue(annotation),
      color,
      onClick: () => emit('annotationClick', annotation),
      data: annotation,
    })
  })
}

/**
 * Get color for annotation value (gradient from blue to red)
 */
const getColorForValue = (value: number, maxValue: number): string => {
  const ratio = value / maxValue

  // Blue to Red gradient
  const blue = { r: 57, g: 126, b: 186 } // #397EBA
  const red = { r: 255, g: 149, b: 149 } // #FF9595

  const r = Math.round(blue.r + (red.r - blue.r) * ratio)
  const g = Math.round(blue.g + (red.g - blue.g) * ratio)
  const b = Math.round(blue.b + (red.b - blue.b) * ratio)

  return `rgb(${r}, ${g}, ${b})`
}

/**
 * Scroll to selected line
 */
const scrollToLine = (line: number) => {
  if (!codeElement.value) {
    console.warn('[SourceCodeEditor] scrollToLine - codeElement not found')
    return
  }

  nextTick(() => {
    if (codeElement.value) {
      scrollToAndHighlight(codeElement.value, line)
    }
  })
}

// Watch for changes
watch(
  () => props.sourceCode,
  (newCode) => {
    if (newCode && !props.loading) {
      highlightCode()
    }
  },
)

watch(
  () => props.annotations,
  () => {
    if (!props.loading && codeElement.value) {
      drawAnnotations()
    }
  },
)

watch(
  () => props.selectedLine,
  (line) => {
    if (line !== null && !props.loading) {
      // Wait for DOM updates and Prism to finish (annotations drawn after 50ms in highlightCode)
      // Use a longer delay to ensure everything is ready
      setTimeout(() => {
        if (codeElement.value) {
          scrollToLine(line)
        } else {
          console.warn('[SourceCodeEditor] codeElement still not ready')
        }
      }, 150) // 150ms to ensure Prism + annotations are ready (50ms + buffer)
    }
  },
)

watch(
  () => props.loading,
  (isLoading) => {
    // When loading finishes, highlight the code
    if (!isLoading && props.sourceCode) {
      highlightCode()
    }
  },
)

// Lifecycle
onMounted(() => {
  // Only highlight if we already have code loaded
  if (props.sourceCode && !props.loading) {
    highlightCode()
  }
})

onBeforeUnmount(() => {
  if (codeElement.value) {
    removeAllAnnotations(codeElement.value)
  }
})

// Expose methods
defineExpose({
  scrollToLine,
  highlightCode,
  drawAnnotations,
})
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.source-code-editor {
  height: 100%;
  overflow: hidden;
  display: flex;
  flex-direction: column;
  margin-top: $spacing-md;
  border: 1px solid $color-border-light;
  border-radius: $radius-lg;
  max-height: 500px;
}

.editor-loading,
.editor-error {
  display: flex;
  align-items: center;
  justify-content: center;
  flex: 1;
  padding: 2rem;
}

.spinner-container {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 1.5rem;
}

.spinner {
  width: 64px;
  height: 64px;
  border: 6px solid #f3f3f3;
  border-top: 6px solid #397eba;
  border-radius: 50%;
  animation: spin 1s linear infinite;
}

@keyframes spin {
  0% {
    transform: rotate(0deg);
  }
  100% {
    transform: rotate(360deg);
  }
}

.loading-text {
  margin: 0;
  font-size: 1rem;
  color: #6a737d;
  font-weight: $font-weight-medium;
}

.editor-error {
  color: $color-error;
  background: $color-error-bg;
  border: 1px solid $color-error-border;
  border-radius: 0.25rem;
  margin: 1rem;
}

.editor-container {
  flex: 1;
  overflow: auto;
  background: $color-bg-primary;
  scrollbar-width: thin;
  scrollbar-color: $color-primary transparent;
}
/* Custom scrollbar styling */

.editor-container::-webkit-scrollbar-thumb {
  background: $color-primary;
  border-radius: $spacing-xs;
}

.editor-container::-webkit-scrollbar-thumb:hover {
  background: $color-primary-light;
}
</style>

<!-- Global Prism styles - consolidated from prism-custom.css -->
<style>
/**
 * Custom Prism.js theme for MALT
 * Based on GitHub syntax coloring (GHColors theme)
 */

/* Base styles for code blocks */
.source-code-editor code[class*='language-'],
.source-code-editor pre[class*='language-'] {
  color: #393a34;
  font-family: 'Consolas', 'Menlo', 'Monaco', 'Courier New', monospace;
  direction: ltr;
  text-align: left;
  white-space: pre;
  word-spacing: normal;
  word-break: normal;
  font-size: 14px;
  line-height: 1.5;
  -moz-tab-size: 4;
  -o-tab-size: 4;
  tab-size: 4;
  -webkit-hyphens: none;
  -moz-hyphens: none;
  -ms-hyphens: none;
  hyphens: none;
}

/* Selection */
.source-code-editor pre[class*='language-']::-moz-selection,
.source-code-editor pre[class*='language-'] ::-moz-selection,
.source-code-editor code[class*='language-']::-moz-selection,
.source-code-editor code[class*='language-'] ::-moz-selection {
  background: #b3d4fc;
}

.source-code-editor pre[class*='language-']::selection,
.source-code-editor pre[class*='language-'] ::selection,
.source-code-editor code[class*='language-']::selection,
.source-code-editor code[class*='language-'] ::selection {
  background: #b3d4fc;
}

/* Code blocks */
.source-code-editor .editor-container pre[class*='language-'] {
  padding: 1em;
  padding-left: 3.8em; /* Space for line numbers */
  margin: 0;
  overflow: auto;
  background: #ffffff;
  border: none;
  min-height: 100%;
  position: relative; /* CRITICAL: For absolute positioning of highlights and annotations */
}

.source-code-editor .editor-container code[class*='language-'] {
  background: transparent;
}

/* Inline code */
.source-code-editor :not(pre) > code[class*='language-'] {
  padding: 0.2em 0.4em;
  background: #f6f8fa;
  border-radius: 3px;
  border: 1px solid #d1d5da;
}

/* Token colors - GitHub style */
.source-code-editor .token.comment,
.source-code-editor .token.prolog,
.source-code-editor .token.doctype,
.source-code-editor .token.cdata {
  color: #6a737d;
  font-style: italic;
}

.source-code-editor .token.namespace {
  opacity: 0.7;
}

.source-code-editor .token.string,
.source-code-editor .token.attr-value {
  color: #032f62;
}

.source-code-editor .token.punctuation,
.source-code-editor .token.operator {
  color: #393a34;
}

.source-code-editor .token.entity,
.source-code-editor .token.url,
.source-code-editor .token.symbol,
.source-code-editor .token.number,
.source-code-editor .token.boolean,
.source-code-editor .token.variable,
.source-code-editor .token.constant,
.source-code-editor .token.property,
.source-code-editor .token.regex,
.source-code-editor .token.inserted {
  color: #36acaa;
}

.source-code-editor .token.atrule,
.source-code-editor .token.keyword,
.source-code-editor .token.attr-name,
.source-code-editor .language-autohotkey .token.selector {
  color: #d73a49;
}

.source-code-editor .token.function,
.source-code-editor .token.deleted,
.source-code-editor .language-autohotkey .token.tag {
  color: #6f42c1;
}

.source-code-editor .token.tag,
.source-code-editor .token.selector,
.source-code-editor .language-autohotkey .token.keyword {
  color: #22863a;
}

.source-code-editor .token.important,
.source-code-editor .token.function,
.source-code-editor .token.bold {
  font-weight: bold;
}

.source-code-editor .token.italic {
  font-style: italic;
}

/* Line numbers plugin */
.source-code-editor pre[class*='language-'].line-numbers {
  position: relative;
  padding-left: 8.9em;
  counter-reset: linenumber;
}

.source-code-editor pre[class*='language-'].line-numbers > code {
  position: relative;
  white-space: inherit;
  z-index: 2; /* Above highlight, below annotations */
}

.source-code-editor .line-numbers .line-numbers-rows {
  position: absolute;
  pointer-events: none;
  top: 0;
  font-size: 100%;
  left: -3.8em;
  width: 3em;
  letter-spacing: -1px;
  border-right: 1px solid #d1d5da;
  -webkit-user-select: none;
  -moz-user-select: none;
  -ms-user-select: none;
  user-select: none;
}

.source-code-editor .line-numbers-rows > span {
  pointer-events: none;
  display: block;
  counter-increment: linenumber;
}

.source-code-editor .line-numbers-rows > span:before {
  content: counter(linenumber);
  color: #6a737d;
  display: block;
  padding-right: 0.8em;
  text-align: right;
}

/* Line highlight plugin */
.source-code-editor .line-highlight {
  position: absolute;
  left: 0;
  right: 0;
  padding: inherit 0;
  margin-top: 0;
  background: rgba(255, 243, 205, 0.4);
  pointer-events: none;
  line-height: inherit;
  white-space: pre;
  z-index: 1; /* Behind code and annotations */
}

/* Code annotations (custom - inline memory annotations) */
.source-code-editor .line-annotate {
  position: absolute;
  left: 0;
  width: 4.5em;
  background-color: rgba(0, 0, 0, 0.7);
  border-radius: 1em;
  margin-top: 1.1em;
  margin-left: 0.5em;
  color: #ffffff;
  text-align: right;
  line-height: 1.2em;
  text-decoration: none;
  cursor: pointer;
  transition: opacity 0.15s ease;
  text-shadow: none;
  text-align: center;
  z-index: 10; /* Above highlight and code */
}

.source-code-editor .line-annotate:hover,
.source-code-editor .line-annotate:focus {
  color: #ffffff;
  text-decoration: none;
  opacity: 0.9;
}

.source-code-editor .line-annotate > span {
  font-size: 0.8em;
  font-weight: bold;
}

/* Annotation size classes */
.source-code-editor .line-annotate.line-annotate-small {
  background-color: #397eba;
}

.source-code-editor .line-annotate.line-annotate-medium {
  background-color: #e99139;
}

.source-code-editor .line-annotate.line-annotate-large {
  background-color: #bf5050;
}

/* Scrollbar styling */
.source-code-editor pre[class*='language-']::-webkit-scrollbar {
  width: 10px;
  height: 10px;
}

.source-code-editor pre[class*='language-']::-webkit-scrollbar-track {
  background: #f1f1f1;
}

.source-code-editor pre[class*='language-']::-webkit-scrollbar-thumb {
  background: #888;
  border-radius: 5px;
}

.source-code-editor pre[class*='language-']::-webkit-scrollbar-thumb:hover {
  background: #555;
}
</style>

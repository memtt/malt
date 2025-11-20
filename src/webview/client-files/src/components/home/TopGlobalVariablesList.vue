<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/home/TopGlobalVariablesList.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->
<template>
  <div class="top-variables-container">
    <!-- Search Input -->
    <div class="search-group">
      <input
        v-model="searchQuery"
        type="text"
        name="global-vars-search"
        placeholder="Search"
        class="form-control"
      />
      <IconSearch class="search-icon" />
    </div>

    <!-- Variables Grid -->
    <div class="variables-grid">
      <div
        v-for="(variable, index) in props.filteredVariables"
        :key="`${variable.binaryFile}-${variable.name}-${variable.size}-${variable.tls}-${index}`"
        class="variable-card"
      >
        <!-- Variable Name and TLS Badge -->
        <div class="variable-header">
          <TooltipWrapper :content="getTooltipContent(variable)" class="variable-name-wrapper">
            <span class="variable-name">{{ truncateName(variable.name) }}</span>
          </TooltipWrapper>
          <span v-if="variable.tls" class="tls-badge">TLS</span>
          <!-- Sources Button -->
          <button
            v-else-if="variable.sourceFile && variable.line"
            class="show-sources-btn"
            @click="goToSources(variable)"
          >
            <IconRedirection class="sources-icon" />
            <span>Sources</span>
          </button>
        </div>

        <!-- Variable Size -->
        <div class="variable-info">
          <span class="info-label">Variable size:</span>
          <span class="info-value">{{ formatSize(variable.totalSize) }}</span>
        </div>

        <!-- Size Used (if different from total size) -->
        <div v-if="variable.usedSize !== undefined" class="variable-info">
          <span class="info-label">Size used:</span>
          <span class="info-value">{{ formatSize(variable.usedSize) }}</span>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { computed } from 'vue'
import { useRouter } from 'vue-router'
import { humanReadable } from '@/lib/helpers'
import type { TopVariable } from '@/utils/globalVars/calculators'
import IconSearch from '@/assets/icons/icon-search.svg?component'
import IconRedirection from '@/assets/icons/icon-redirection.svg?component'
import TooltipWrapper from '@/components/core/TooltipWrapper.vue'

type Props = {
  filteredVariables: TopVariable[]
  searchQuery: string
}

const props = defineProps<Props>()
const emit = defineEmits<{
  'update:searchQuery': [value: string]
}>()

const router = useRouter()

const searchQuery = computed({
  get: () => props.searchQuery,
  set: (value: string) => emit('update:searchQuery', value),
})

function formatSize(bytes: number): string {
  return humanReadable(bytes, 1, 'B', true)
}

function truncateName(name: string): string {
  const maxLength = 30
  if (name.length <= maxLength) {
    return name
  }
  return name.substring(0, maxLength) + '...'
}

function getShortBinaryName(binaryPath: string): string {
  // Extract just the filename from the full path
  const parts = binaryPath.split('/')
  return parts[parts.length - 1] || binaryPath
}

function getTooltipContent(variable: TopVariable): string {
  const shortBinary = getShortBinaryName(variable.binaryFile)
  return `${shortBinary}: ${variable.name}`
}

function goToSources(variable: TopVariable): void {
  if (variable.sourceFile && variable.line) {
    router.push({
      name: 'Sources',
      query: {
        file: variable.sourceFile,
        line: variable.line.toString(),
      },
    })
  }
}
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.top-variables-container {
  display: flex;
  flex-direction: column;
  gap: $spacing-lg;
}

.search-group {
  position: relative;
  display: flex;
  max-width: 320px;
}

.form-control {
  display: flex;
  border-radius: $radius-md;
  border-width: 1px;
  padding: $spacing-sm 40px $spacing-sm $spacing-md;
  gap: $spacing-sm;
  width: 100%;
  background: $color-bg-primary;
  border: 1px solid $color-border-medium;
  color: $color-primary;
  line-height: 20px;
  font-size: $font-size-base;
  font-weight: $font-weight-medium;
}

.search-icon {
  position: absolute;
  right: $spacing-md;
  top: 50%;
  transform: translateY(-50%);
  width: 20px;
  height: 20px;
  color: $color-primary;
  opacity: 0.6;
  pointer-events: none;
}

.form-control::placeholder {
  color: $color-primary;
  opacity: 0.6;
}

.form-control:focus {
  outline: none;
  border-color: $color-primary-light;
}

.variables-grid {
  display: grid;
  grid-template-columns: repeat(4, 1fr);
  gap: $spacing-md;
}

@media (min-width: 1200px) {
  .variables-grid {
    grid-template-columns: repeat(5, 1fr);
  }
}

@media (max-width: 1100px) {
  .variables-grid {
    grid-template-columns: repeat(3, 1fr);
  }
}

@media (max-width: 800px) {
  .variables-grid {
    grid-template-columns: repeat(2, 1fr);
  }
}

@media (max-width: 500px) {
  .variables-grid {
    grid-template-columns: 1fr;
  }
}

.variable-card {
  display: flex;
  flex-direction: column;
  gap: $spacing-lg;
  padding: $spacing-lg;
  border: 1px solid $color-border-light;
  border-radius: $radius-lg;
  background: $color-bg-primary;
}

.variable-header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: $spacing-md;
  min-height: 24px;
}

.variable-name-wrapper {
  flex: 1;
  min-width: 0;
}

.variable-name {
  font-size: $font-size-md;
  font-weight: $font-weight-normal;
  color: $color-primary;
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
  display: block;
}

.tls-badge {
  display: inline-flex;
  align-items: center;
  padding: 2px $spacing-sm;
  background: $color-bg-lighter;
  border: 1px solid $color-primary;
  border-radius: $radius-lg;
  font-size: 11px;
  font-weight: $font-weight-bold;
  color: $color-primary;
  text-transform: uppercase;
  flex-shrink: 0;
}

.variable-info {
  display: flex;
  flex-direction: column;
  gap: 2px;
}

.info-label {
  font-size: $font-size-xs;
  font-weight: $font-weight-medium;
  color: $color-text-muted;
}

.info-value {
  font-size: $font-size-sm;
  font-weight: $font-weight-bold;
  color: $color-text-primary;
}

.show-sources-btn {
  display: flex;
  align-items: center;
  gap: 6px;
  background: transparent;
  color: $color-primary-light;
  border: none;
  border-radius: $radius-md;
  padding: 6px $spacing-md;
  font-family: $font-family-base;
  font-size: $font-size-base;
  font-weight: $font-weight-normal;
  cursor: pointer;
  transition: all $transition-base ease;
}

.show-sources-btn:hover {
  background: rgba(59, 130, 246, 0.1);
}

.show-sources-btn span {
  text-decoration: underline;
}

.sources-icon {
  width: 18px;
  height: 18px;
  flex-shrink: 0;
  color: $color-primary-light;
}
</style>

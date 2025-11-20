<template>
  <BaseModal
    :model-value="props.modelValue"
    no-header
    container-class="help-modal-container"
    @update:model-value="$emit('update:modelValue', $event)"
  >
    <div class="help-content">
      <h2 class="help-title">
        <span class="title-normal">{{ props.titlePrefix }}</span>
        <span v-if="props.titleHighlight" class="title-highlight">{{ props.titleHighlight }}</span>
      </h2>

      <div class="help-sections">
        <div v-for="(section, sectionIndex) in sections" :key="sectionIndex" class="help-section">
          <div class="section-header">
            <component :is="section.icon" v-if="section.icon" class="section-icon" />
            <h3 class="section-title">{{ section.title }}</h3>
          </div>

          <div class="help-items">
            <div v-for="(item, itemIndex) in section.items" :key="itemIndex" class="help-item">
              <div class="help-item-label">
                {{ item.label }}
              </div>
              <div class="help-item-content">
                <div class="help-item-description">
                  {{ item.description }}
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </BaseModal>
</template>

<script setup lang="ts">
import { computed } from 'vue'
import type { Component, FunctionalComponent, SVGAttributes } from 'vue'
import BaseModal from '@/components/core/BaseModal.vue'
import { useHelpSections } from '@/composables/useHelpSections'

export interface HelpItem {
  label: string
  description: string
}

export interface HelpSection {
  title: string
  icon?: Component | FunctionalComponent<SVGAttributes>
  items: HelpItem[]
}

interface Props {
  modelValue: boolean
  titlePrefix: string
  titleHighlight?: string
  sections?: HelpSection[]
}

const props = defineProps<Props>()

defineEmits<{
  'update:modelValue': [value: boolean]
}>()

// Use default help sections if not provided
const defaultSections = useHelpSections()
const sections = computed(() => props.sections ?? defaultSections)
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.help-content {
  padding: $spacing-3xl;
}

.help-title {
  font-family: $font-family-base;
  font-size: $font-size-4xl;
  font-weight: $font-weight-normal;
  line-height: 120%;
  margin: 0 0 $spacing-2xl 0;
  color: $color-text-primary;
}

.title-normal {
  font-weight: $font-weight-normal;
}

.title-highlight {
  font-weight: $font-weight-bold;
  margin-left: $spacing-sm;
}

.help-sections {
  display: flex;
  flex-direction: column;
  gap: $spacing-md;
}

.help-section {
  border-radius: $radius-lg;
  padding: $spacing-lg;
  border: 1px solid $color-border-light;
  background: $gradient-card;
}

.section-header {
  display: flex;
  align-items: center;
  gap: $spacing-sm;
  margin-bottom: $spacing-lg;
}

.section-icon {
  width: 20px;
  height: 20px;
  color: $color-primary;
}

.section-title {
  font-family: $font-family-base;
  font-size: $font-size-md;
  font-weight: $font-weight-normal;
  line-height: 20px;
  margin: 0;
  color: $color-primary;
}

.help-items {
  display: flex;
  flex-direction: column;
  gap: $spacing-md;
}

.help-item-label {
  font-family: $font-family-base;
  font-size: $font-size-xs;
  font-weight: $font-weight-medium;
  color: $color-text-primary;
}

.help-item-content {
  display: flex;
  flex-direction: column;
  gap: $spacing-xs;
}

.help-item-description {
  font-family: $font-family-base;
  font-size: $font-size-base;
  font-weight: $font-weight-bold;
  color: $color-text-primary;
}

/* Responsive */
@media (max-width: 768px) {
  .help-content {
    padding: $spacing-xl;
  }

  .help-title {
    font-size: $font-size-2xl;
    margin-bottom: $spacing-xl;
  }

  .help-item {
    grid-template-columns: 1fr;
    gap: $spacing-sm;
  }

  .section-title {
    font-size: $font-size-md;
  }
}
</style>

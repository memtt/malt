<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/core/PageSection.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->
<template>
  <section class="page-section">
    <h2 class="section-title">
      <slot name="title">{{ title }}</slot>
    </h2>
    <div v-if="$slots.action" class="section-action" :class="{ inline: actionInline }">
      <slot name="action"></slot>
    </div>
    <div class="section-content">
      <slot></slot>
    </div>
  </section>
</template>

<script setup lang="ts">
interface Props {
  title?: string
  showHelp?: boolean
  actionInline?: boolean
}

withDefaults(defineProps<Props>(), {
  title: '',
  showHelp: false,
  actionInline: false,
})
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.page-section {
  position: relative;
  margin: 0 auto $spacing-md;
  padding: $spacing-2xl $spacing-3xl $spacing-3xl;
  background: rgba(255, 255, 255, 0.8);
  border: 1px solid $color-border-light;
  border-radius: $radius-xl;
  display: flex;
  flex-direction: column;
  gap: $spacing-md;
  box-sizing: border-box;
  overflow: auto;
}

.section-title {
  font-family: $font-family-base;
  font-weight: $font-weight-bold;
  font-size: 30px;
  line-height: 36px;
  margin: 0;
  color: $color-primary;
}

.section-title :deep(span) {
  font-family: $font-family-base;
  font-weight: $font-weight-normal;
  font-size: 30px;
  line-height: 36px;
}

.section-action {
  position: absolute;
  top: $spacing-2xl;
  right: $spacing-3xl;
}

.section-action.inline {
  position: static;
  margin-bottom: $spacing-xs;
}

.section-content {
  display: flex;
  flex-wrap: wrap;
  gap: $spacing-md;
}

/* Four columns variant for Global statistics */
.page-section.four-columns .section-content {
  display: grid;
  grid-template-columns: repeat(4, 1fr);
  gap: $spacing-xl;
}

.page-section.four-columns .section-content > * {
  min-width: 0;
}

@media (max-width: 1024px) {
  .page-section.four-columns .section-content {
    grid-template-columns: repeat(3, 1fr);
  }
}

@media (max-width: 768px) {
  .page-section {
    padding: $spacing-xl 20px;
    border-radius: $radius-lg;
  }

  .section-title {
    font-size: $font-size-2xl;
    line-height: 30px;
  }

  .section-content {
    gap: $spacing-lg;
  }

  .page-section.four-columns .section-content {
    grid-template-columns: 1fr;
    gap: $spacing-lg;
  }
}
</style>

<template>
  <div class="file-header">
    <div class="file-path">{{ filePath }}</div>
    <div v-if="functionName" class="function-name">
      {{ functionName }}
    </div>
  </div>
</template>

<script setup lang="ts">
defineProps<{
  filePath: string | null
  functionName?: string | null
}>()
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.file-path {
  font-weight: $font-weight-medium;
  font-size: $font-size-xs;
  color: $color-primary;
}

.function-name {
  font-weight: $font-weight-bold;
  font-size: 30px;
  line-height: 36px;
  color: $color-text-primary;
}
</style>

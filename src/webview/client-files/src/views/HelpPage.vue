<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/views/HelpPage.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->
<template>
  <div class="help-page">
    <div class="help-card">
      <h1 class="help-title">Help & Documentation</h1>
    </div>
  </div>
</template>

<script setup lang="ts">
// Help content logic will be implemented here
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.help-page {
  display: flex;
  flex-direction: column;
  gap: $spacing-2xl;
}

.help-card {
  background: $color-bg-primary;
  border-radius: $radius-lg;
  box-shadow: $shadow-card;
  padding: $spacing-2xl;
}

.help-title {
  font-size: $font-size-3xl;
  font-weight: $font-weight-bold;
  color: $gray-900;
  margin-bottom: $spacing-md;
}
</style>

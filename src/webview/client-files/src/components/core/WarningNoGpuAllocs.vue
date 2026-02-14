<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/core/TooltipWrapper.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->
<template>
  <div class="no-source-warning" role="alert">
    <h3 class="warning-title">No GPU memory allocation detected</h3>
    <p class="warning-message">
      The MALT profile has not detected any GPU memory allocations (cudaMalloc, hipMalloc, OpenACC allocations...).
    </p>
    <p class="warning-message">
      If your code is expected to make some GPU memory allocation, check that you compiled your CUDA code with
      <strong>--cudart shared</strong>
    </p>
  </div>
</template>

<script setup lang="ts">
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.no-source-warning {
  width: 100%;
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

.selection-warning {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  background: #fffcf5;
  border: 2px solid #ffab6b;
  border-radius: $radius-md;
  padding: 10px;
  margin-bottom: $spacing-lg;
  text-align: center;
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

</style>

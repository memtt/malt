<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/views/ReallocPage.vue
|-----------------------------------------------------------
|    AUTHOR   : Sébastien Valat - 2026
----------------------------------------------------------->
<template>
  <div class="static-no-avail-page">
    <!-- Loading State -->
    <div class="info-container">
      <h1>Not available in static mode !</h1>
      <p>This page is not available in static mode.</p>
      <p>If you want to use it, you need to download <b><a :href='maltProfilePath' target="_blank" download>this profile JSON file</a></b> and using the <b>malt-webiew</b> to browser it.</p>
      <h2>Getting MALT</h2>
      <p>To get MALT, download it on <b><a href='https://memtt.github.io/malt' target="_blank">https://memtt.github.io/malt</a></b> and install it following the <b><a href='https://memtt.github.io/malt/doc/latest/index.html' target="_blank">documentation</a></b>.</p>
      <h2>Running malt-webview</h2>
      <p>Then just launch the command :</p>
      <pre>xz -d ./{{ maltProfilePath.split("/")[1] }}</pre>
      <pre>malt-webview ./malt-profile.json</pre>
      <p>And open you browser at <b><a href='http://localhost:8080'>http://localhost:8080</a></b></p>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, } from 'vue'
declare var MALT_PROFILE_PATH: any;
const maltProfilePath = MALT_PROFILE_PATH;
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.realloc-page {
  margin: 0 auto;
}

/* Loading State */
.info-container {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  padding: $spacing-5xl 20px;
  background-color: $blue-section-bg;
  border-radius: $radius-xl;
}

</style>

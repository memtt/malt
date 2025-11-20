<template>
  <div v-if="isReady" class="app-layout">
    <AppHeader v-if="!isLoginPage" />
    <main :class="isLoginPage ? '' : 'main-content'">
      <div class="content-wrapper">
        <router-view />
      </div>
    </main>
  </div>
</template>

<script setup lang="ts">
import { computed, ref, onMounted } from 'vue'
import { useRoute, useRouter } from 'vue-router'
import AppHeader from '@/components/core/AppHeader.vue'

const route = useRoute()
const router = useRouter()
const isReady = ref(false)

const isLoginPage = computed(() => route.path === '/login')

onMounted(() => {
  // Wait for router to be ready
  router.isReady().then(() => {
    isReady.value = true
  })
})
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.app-layout {
  min-height: 100vh;
  background: $gradient-primary;
}

.main-content {
  padding: $spacing-2xl $spacing-5xl;
}

.content-wrapper {
  max-width: 100%;
  overflow-x: hidden;
}

/* Responsive: Below 1024px */
@media (max-width: 1024px) {
  .main-content {
    padding: $spacing-2xl $spacing-2xl;
  }
}
</style>

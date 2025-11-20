<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/views/LoginPage.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->
<template>
  <div class="login-page">
    <div class="login-container">
      <!-- Logo -->
      <div class="login-header">
        <h1 class="login-title">
          <span class="title-bold">MALT</span>
          <span class="title-normal"> WebView</span>
        </h1>
        <p class="login-subtitle">Enter your access token to continue</p>
      </div>

      <!-- Error message if redirected from 401/403 -->
      <div v-if="errorMessage" class="error-banner">
        <span class="error-icon">⚠</span>
        <span>{{ errorMessage }}</span>
      </div>

      <!-- Login form -->
      <form @submit.prevent="handleSubmit" class="login-form">
        <div class="form-group">
          <label for="token" class="form-label">Access Token</label>
          <input
            id="token"
            v-model="tokenInput"
            type="text"
            class="form-input"
            placeholder="Enter your access token"
            autocomplete="off"
            required
          />
        </div>

        <!-- Submit button -->
        <button type="submit" class="submit-btn" :disabled="!tokenInput.trim()">
          <span v-if="!isSubmitting">Access WebView</span>
          <span v-else>Authenticating...</span>
        </button>

        <!-- Validation error -->
        <p v-if="validationError" class="validation-error">{{ validationError }}</p>
      </form>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted } from 'vue'
import { useRouter, useRoute } from 'vue-router'
import { useAuthStore } from '@/stores/authStore'

const router = useRouter()
const route = useRoute()
const authStore = useAuthStore()

const tokenInput = ref('')
const isSubmitting = ref(false)
const validationError = ref('')
const errorMessage = ref('')

// Check for error reason in query params
onMounted(() => {
  const reason = route.query.reason as string
  if (reason === 'unauthorized') {
    errorMessage.value = 'Your session has expired or you are not authorized. Please log in again.'
  } else if (reason === 'required') {
    errorMessage.value = 'Authentication is required to access this page.'
  }
})

const handleSubmit = () => {
  validationError.value = ''

  // Validate token
  if (!tokenInput.value.trim()) {
    validationError.value = 'Please enter a valid token'
    return
  }

  // Set token in store
  const success = authStore.setToken(tokenInput.value.trim())

  if (success) {
    // Redirect to the page user was trying to access, or home by default
    const redirectTo = (route.query.redirect as string) || '/home'
    router.push(redirectTo)
  } else {
    validationError.value = 'Invalid token format'
  }
}
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.login-page {
  min-height: 100vh;
  display: flex;
  align-items: center;
  justify-content: center;
  background: $gradient-login;
  padding: 20px;
}

.login-container {
  background: $color-bg-primary;
  border-radius: $radius-xl;
  padding: $spacing-4xl;
  box-shadow: $shadow-card;
  width: 100%;
  max-width: 440px;
}

.login-header {
  text-align: center;
  margin-bottom: $spacing-2xl;
}

.login-title {
  font-size: $font-size-4xl;
  line-height: 1.2;
  color: $color-text-primary;
  margin-bottom: $spacing-sm;
}

.title-bold {
  font-weight: $font-weight-bold;
}

.title-normal {
  font-weight: $font-weight-normal;
}

.login-subtitle {
  font-size: $font-size-md;
  color: $color-text-secondary;
  font-weight: $font-weight-normal;
  margin: 0;
}

.error-banner {
  display: flex;
  align-items: center;
  gap: $spacing-md;
  background-color: $color-error-bg;
  border: 1px solid $color-error-border;
  border-radius: $radius-lg;
  padding: $spacing-lg;
  margin-bottom: $spacing-xl;
  color: $color-error;
  font-size: $font-size-sm;
  line-height: 1.5;
}

.error-icon {
  font-size: $font-size-xl;
  flex-shrink: 0;
}

.login-form {
  display: flex;
  flex-direction: column;
  gap: $spacing-xl;
}

.form-group {
  display: flex;
  flex-direction: column;
  gap: $spacing-sm;
}

.form-label {
  font-size: $font-size-base;
  font-weight: $font-weight-medium;
  color: $color-text-primary;
}

.form-input {
  width: 100%;
  padding: $spacing-md $spacing-lg;
  border: 1px solid $color-border-medium;
  border-radius: $radius-lg;
  font-size: $font-size-base;
  color: $color-text-primary;
  background: $color-bg-primary;
  transition: all $transition-base $transition-ease;
  font-family: $font-family-base;
}

.form-input:focus {
  outline: none;
  border-color: $color-primary-light;
  box-shadow: 0 0 0 3px rgba(26, 125, 175, 0.1);
}

.form-input::placeholder {
  color: #9ca3af;
}

.submit-btn {
  width: 100%;
  padding: 14px $spacing-xl;
  background-color: $color-primary;
  color: white;
  border: none;
  border-radius: $radius-lg;
  font-size: $font-size-md;
  font-weight: $font-weight-semibold;
  cursor: pointer;
  transition: all $transition-base $transition-ease;
  font-family: $font-family-base;
}

.submit-btn:hover:not(:disabled) {
  background-color: $color-primary-light;
  transform: translateY(-1px);
  box-shadow: 0 4px 12px rgba(24, 72, 98, 0.2);
}

.submit-btn:disabled {
  opacity: 0.5;
  cursor: not-allowed;
  transform: none;
}

.validation-error {
  color: $color-error;
  font-size: $font-size-sm;
  margin: -$spacing-md 0 0 0;
  text-align: center;
}

/* Responsive */
@media (max-width: 480px) {
  .login-container {
    padding: $spacing-2xl $spacing-xl;
  }

  .login-title {
    font-size: $font-size-3xl;
  }
}
</style>

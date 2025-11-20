import { defineStore } from 'pinia'
import { ref } from 'vue'

const TOKEN_STORAGE_KEY = 'malt_auth_token'

export const useAuthStore = defineStore('auth', () => {
  // State
  const token = ref<string | null>(null)

  // Initialize token from localStorage on store creation
  const initToken = () => {
    const storedToken = localStorage.getItem(TOKEN_STORAGE_KEY)
    if (storedToken) {
      token.value = storedToken
    }
  }

  // Actions
  const setToken = (newToken: string) => {
    if (!newToken || newToken.trim() === '') {
      return false
    }
    token.value = newToken
    localStorage.setItem(TOKEN_STORAGE_KEY, newToken)
    return true
  }

  const clearToken = () => {
    token.value = null
    localStorage.removeItem(TOKEN_STORAGE_KEY)
  }

  const getToken = (): string | null => {
    return token.value
  }

  const isAuthenticated = (): boolean => {
    return token.value !== null && token.value !== ''
  }

  // Initialize token when store is created
  initToken()

  return {
    token,
    setToken,
    clearToken,
    getToken,
    isAuthenticated,
  }
})

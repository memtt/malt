/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/router/index.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
import { createRouter, createWebHistory } from 'vue-router'
import { useAuthStore } from '@/stores/authStore'

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/',
      redirect: () => {
        const authStore = useAuthStore()
        return authStore.isAuthenticated() ? '/home' : '/login'
      },
    },
    {
      path: '/login',
      name: 'Login',
      component: () => import('@/views/LoginPage.vue'),
      meta: { requiresAuth: false },
    },
    {
      path: '/home',
      name: 'Home',
      component: () => import('@/views/HomePage.vue'),
      meta: { requiresAuth: true },
    },
    {
      path: '/per-thread',
      name: 'Threads',
      component: () => import('@/views/PerThreadPage.vue'),
      meta: { requiresAuth: true },
    },
    {
      path: '/sources',
      name: 'Sources',
      component: () => import('@/views/SourcesPage.vue'),
      meta: { requiresAuth: true },
    },
    {
      path: '/call-tree',
      name: 'Calltree',
      component: () => import('@/views/CallTreePage.vue'),
      meta: { requiresAuth: true },
    },
    {
      path: '/timeline',
      name: 'Timeline analysis',
      component: () => import('@/views/TimelinePage.vue'),
      meta: { requiresAuth: true },
    },
    {
      path: '/stackPeaks',
      name: 'Stack memory',
      component: () => import('@/views/StackPeaksPage.vue'),
      meta: { requiresAuth: true },
    },
    {
      path: '/allocSizeDistr',
      name: 'Alloc sizes',
      component: () => import('@/views/AllocSizeDistrPage.vue'),
      meta: { requiresAuth: true },
    },
    {
      path: '/realloc',
      name: 'Realloc',
      component: () => import('@/views/ReallocPage.vue'),
      meta: { requiresAuth: true },
    },
    {
      path: '/globalVars',
      name: 'Global variables',
      component: () => import('@/views/GlobalVarsPage.vue'),
      meta: { requiresAuth: true },
    },
    {
      path: '/help',
      name: 'Help',
      component: () => import('@/views/HelpPage.vue'),
      meta: { requiresAuth: true },
    },
  ],
})

// Navigation guard for authentication
router.beforeEach((to, from, next) => {
  const authStore = useAuthStore()
  const requiresAuth = to.meta.requiresAuth !== false // Default to true if not specified

  if (requiresAuth && !authStore.isAuthenticated()) {
    // Redirect to login with the page user was trying to access
    next({
      path: '/login',
      query: { redirect: to.fullPath, reason: 'required' },
    })
  } else if (to.path === '/login' && authStore.isAuthenticated()) {
    // If already authenticated and trying to access login, redirect to home
    next('/home')
  } else {
    next()
  }
})

export default router

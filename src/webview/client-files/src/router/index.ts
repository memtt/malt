/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/router/index.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
import { createRouter, createWebHashHistory } from 'vue-router'
import { useAuthStore } from '@/stores/authStore'

function genRoutes(): Array<any>
{
  var routes = [];
  if (process.env.VITE_APP == 'static' || process.env.VITE_APP == 'summary') {
    routes.push({
      path: '/',
      redirect: () => {
        return '/home'
      },
    });
  } else {
    routes.push({
      path: '/',
      redirect: () => {
      const authStore = useAuthStore()
        return authStore.isAuthenticated() ? '/home' : '/login'
      },
    });
  }
  routes.push({
    path: '/home',
    name: 'Home',
    component: () => import('@/views/HomePage.vue'),
    meta: { requiresAuth: true },
  });
  if (process.env.VITE_APP != 'summary') {
    routes.push({
      path: '/login',
      name: 'Login',
      component: () => import('@/views/LoginPage.vue'),
      meta: { requiresAuth: false },
    });
    routes.push({
      path: '/per-thread',
      name: 'Threads',
      component: () => import('@/views/PerThreadPage.vue'),
      meta: { requiresAuth: true },
    });
    if (process.env.VITE_APP != 'static') {
      routes.push({
        path: '/sources',
        name: 'Sources',
        component: () => import('@/views/SourcesPage.vue'),
        meta: { requiresAuth: true },
      });
      routes.push({
        path: '/call-tree',
        name: 'Calltree',
        component: () => import('@/views/CallTreePage.vue'),
        meta: { requiresAuth: true },
      });
    }
    routes.push({
      path: '/timeline',
      name: 'Timeline analysis',
      component: () => import('@/views/TimelinePage.vue'),
      meta: { requiresAuth: true },
    });
    routes.push({
      path: '/stackPeaks',
      name: 'Stack memory',
      component: () => import('@/views/StackPeaksPage.vue'),
      meta: { requiresAuth: true },
    });
    routes.push({
      path: '/allocSizeDistr',
      name: 'Alloc sizes',
      component: () => import('@/views/AllocSizeDistrPage.vue'),
      meta: { requiresAuth: true },
    });
    routes.push({
      path: '/realloc',
      name: 'Realloc',
      component: () => import('@/views/ReallocPage.vue'),
      meta: { requiresAuth: true },
    });
    routes.push({
      path: '/globalVars',
      name: 'Global variables',
      component: () => import('@/views/GlobalVarsPage.vue'),
      meta: { requiresAuth: true },
    });
  }
  routes.push({
    path: '/help',
    name: 'Help',
    component: () => import('@/views/HelpPage.vue'),
    meta: { requiresAuth: true },
  });

  if (process.env.VITE_APP == 'static' || process.env.VITE_APP == 'summary') {
    const hiddenPages: any = {
      "static": ['/sources', '/call-tree',],
      "summary": ['/sources', '/call-tree',],
    };

    hiddenPages[process.env.VITE_APP].forEach( (element: string) => {
      routes.push({
        path: element,
        name: 'Not vailable page '+element,
        component: () => import('@/views/StaticNotAvailPage.vue'),
        meta: { requiresAuth: true },
      });
    });
  }
  return routes;
}

const router = createRouter({
  history: createWebHashHistory(import.meta.env.BASE_URL),
  routes: genRoutes()
})

// Navigation guard for authentication
router.beforeEach((to, from, next) => {
  const authStore = useAuthStore()
  const requiresAuth = to.meta.requiresAuth !== false // Default to true if not specified
  
  var static_app: boolean = false;
  console.log(process.env.VITE_APP);
  if (process.env.VITE_APP == 'static' || process.env.VITE_APP == 'summary') {
    if (to.path === '/login') {
      next("/home");
    /*} else if (enabledPages[process.env.VITE_APP][to.path] == undefined) {
      next("/staticNotAvailPage");*/
    } else {
      next();
    }
  } else {
    if (requiresAuth && !authStore.isAuthenticated() && static_app == false) {
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
  }
})

export default router

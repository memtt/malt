<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/core/AppHeader.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
|    AUTHOR   : Sébastien Valat (INRIA) - 2025
----------------------------------------------------------->
<template>
  <header class="app-header">
    <div class="header-container">
      <!-- Logo -->
      <router-link to="/home" class="logo-link">
        <h1 class="logo-title">
          <span class="font-bold">MALT</span>
          <span class="font-normal"> WebView</span>
        </h1>
      </router-link>

      <!-- Navigation Menu (Pill-shaped) -->
      <nav class="nav-menu">
        <router-link
          v-for="route in navigationRoutes"
          :key="route.path"
          :to="route.path"
          class="nav-link"
          :class="{ active: $route.path === route.path }"
        >
          {{ route.name }}
        </router-link>
      </nav>
    </div>
  </header>
</template>

<script setup lang="ts">
function genNavigationRoutes() : Array<any>
{
	//vars
	var routes = [];

	//basic
	routes.push({ path: '/home', name: 'Home' });

	//only for dynamic
	if (process.env.VITE_APP != 'static' && process.env.VITE_APP != 'summary') {
		routes.push({ path: '/sources', name: 'Sources' });
		routes.push({ path: '/call-tree', name: 'Calltree' });
	}

	//for all
	if (process.env.VITE_APP != 'summary') {
		routes.push({ path: '/timeline', name: 'Timeline Analysis' });
		routes.push({ path: '/allocSizeDistr', name: 'Alloc Sizes' });
		routes.push({ path: '/globalVars', name: 'Global Variables' });
		routes.push({ path: '/stackPeaks', name: 'Stack Memory' });
		routes.push({ path: '/per-thread', name: 'Threads' });
		routes.push({ path: '/realloc', name: 'Realloc' });
	}

	//ok
	return routes;
}

const navigationRoutes = genNavigationRoutes();
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.app-header {
  padding: $spacing-2xl $spacing-5xl 0;
  min-width: 900px;
}

.header-container {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: $spacing-lg;
  flex-wrap: wrap;
}

.logo-link {
  text-decoration: none;
  cursor: pointer;
  transition: opacity $transition-base $transition-ease;
  flex-shrink: 0;
}

.logo-link:hover {
  opacity: 0.8;
}

.logo-title {
  font-size: $font-size-xl;
  line-height: 1;
  white-space: nowrap;
  color: $color-header-title;
}

.nav-menu {
  border-radius: $radius-full;
  padding: $spacing-lg $spacing-xl;
  display: flex;
  align-items: center;
  gap: $spacing-lg;
  background: $color-bg-primary;
  border: 1px solid $color-border-light;
  flex-wrap: nowrap;
  overflow-x: auto;
  max-width: 100%;
}

.nav-link {
  font-size: $font-size-base;
  line-height: 1.25;
  white-space: nowrap;
  color: $color-text-secondary;
  font-weight: $font-weight-normal;
  transition: color $transition-base $transition-ease;
  text-decoration: none;
  flex-shrink: 0;
}

.nav-link.active {
  color: $color-text-primary;
  font-weight: $font-weight-medium;
}

/* Responsive: Below 1200px */
@media (max-width: 1200px) {
  .nav-menu {
    gap: $spacing-md;
    padding: $spacing-md $spacing-lg;
  }

  .nav-link {
    font-size: $font-size-sm;
  }
}

/* Responsive: Below 1024px - Stack menu under logo */
@media (max-width: 1024px) {
  .header-container {
    flex-direction: column;
    align-items: flex-start;
  }

  .nav-menu {
    justify-content: flex-start;
  }

  .app-header {
    padding: $spacing-xl $spacing-2xl 0;
  }
}
</style>

/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/vite.config.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
*    AUTHOR   : Sébastien Valat - 2026
***********************************************************/

import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'
import { resolve } from 'node:path'
import svgLoader from 'vite-svg-loader'
import path from "path";
import { viteSingleFile } from "vite-plugin-singlefile"
import { env } from 'node:process';

function gen_build_path() : string
{
    if (process.env.MALT_VITE_BUILD_DIR != undefined)
        return <string>process.env.MALT_VITE_BUILD_DIR;
    else
        return "./";
}

function gen_orig_path() : string
{
    if (process.env.MALT_VITE_ORIG_DIR != undefined) {
        console.log(<string>process.env.MALT_VITE_ORIG_DIR);
        return <string>process.env.MALT_VITE_ORIG_DIR;
    } else {
        return __dirname;
    }
}

function gen_out_dir() : string
{
    if (process.env.VITE_APP === 'static') {
        return "dist/static";
    } else if (process.env.VITE_APP === 'summary') {
        return "dist/summary";
    } else {
        return "dist/dynamic";
    }
}

function gen_plugins() : Array<any> {
  var plugins = [];
  plugins.push(vue());
  plugins.push(svgLoader({
      svgoConfig: {
      plugins: [
        {
          name: 'prefixIds',
          params: {
              // Generate unique IDs for each SVG instance
              delim: '_',
              prefix: () => Math.random().toString(36).substring(2, 9),
          },
        },
      ],
    },
  }));
  if (process.env.VITE_APP === 'static' || process.env.VITE_APP === 'summary') {
    plugins.push(viteSingleFile());
  }
  return plugins;
}

function gen_base_dir(): string
{
  if (process.env.VITE_APP === 'static' || process.env.VITE_APP === 'summary') {
    return "";
  } else {
    return "/";
  }
}

export default defineConfig({
  base: gen_base_dir(),
  plugins: gen_plugins(),
  resolve: {
    alias: {
      '@': resolve(__dirname, 'src'),
    },
  },
  define: {
    'process.env': {
      VITE_APP: process.env.VITE_APP,
    }
  },
  build: {
    outDir: path.join(gen_build_path(), gen_out_dir()),
    emptyOutDir: true, // also necessary
    //minify: false,
    rollupOptions: {
      output: {
        // Disable code splitting - generate single JS and CSS files
        manualChunks: undefined,
        inlineDynamicImports: true,
        entryFileNames: 'assets/[name].js',
        chunkFileNames: 'assets/[name].js',
        assetFileNames: 'assets/[name].[ext]',
      },
    },
    cssCodeSplit: false,
  },
  server: {
    proxy: {
      // Proxy API requests to the backend server
      '/global-variables.json': {
        target: 'http://localhost:8080',
        changeOrigin: true,
      },
      '/data/summary.json': {
        target: 'http://localhost:8080',
        changeOrigin: true,
      },
      '/timed.json': {
        target: 'http://localhost:8080',
        changeOrigin: true,
      },
      '/stacks-mem.json': {
        target: 'http://localhost:8080',
        changeOrigin: true,
      },
      '/stack.json': {
        target: 'http://localhost:8080',
        changeOrigin: true,
      },
      '/flat.json': {
        target: 'http://localhost:8080',
        changeOrigin: true,
      },
      '/scatter.json': {
        target: 'http://localhost:8080',
        changeOrigin: true,
      },
      '/size-map.json': {
        target: 'http://localhost:8080',
        changeOrigin: true,
      },
      '/realloc-map.json': {
        target: 'http://localhost:8080',
        changeOrigin: true,
      },
      '/source-file': {
        target: 'http://localhost:8080',
        changeOrigin: true,
      },
      '/file-infos.json': {
        target: 'http://localhost:8080',
        changeOrigin: true,
      },
      '/call-stack-next-level.json': {
        target: 'http://localhost:8080',
        changeOrigin: true,
      },
      '/calltree': {
        target: 'http://localhost:8080',
        changeOrigin: true,
      },
    },
  },
})

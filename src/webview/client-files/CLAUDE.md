# MALT Project - Claude Memory

## Overview

MALT (Memory Analysis and Leak Tracker) is a Vue 3 + TypeScript application for analyzing memory profiling data. It provides visualization and analysis tools for memory allocations, reallocs, global variables, stack peaks, and call trees.

## Architecture

### Tech Stack

- **Frontend Framework**: Vue 3.5+ (Composition API)
- **Language**: TypeScript 5.8
- **Build Tool**: Vite 7
- **Styling**: Pure SCSS with custom utility classes (migrated from Tailwind)
- **State Management**: Pinia 3 (with persisted state plugin)
- **Routing**: Vue Router 4.5
- **Data Fetching**: TanStack Query (Vue Query) 5.90
- **Data Visualization**: D3.js 7.9
- **Validation**: Zod 4.1
- **Utilities**: VueUse 13.9

### Node Version

- Node: ^20.19.0 || >=22.12.0

### Project Structure

```
/home/rico/workspace/malt/
├── webview/
│   └── client-files/          # Vue application root
│       ├── src/
│       │   ├── components/    # Reusable UI components (organized by domain)
│       │   │   ├── core/      # Core/layout components (AppHeader, BaseModal, etc.)
│       │   │   ├── charts/    # D3 chart components (DonutChart, ScatterChart, etc.)
│       │   │   ├── home/      # HomePage-specific components
│       │   │   ├── call-tree/ # CallTree page components
│       │   │   ├── sources/   # Sources page components
│       │   │   └── shared/    # Shared components across pages
│       │   ├── composables/   # Vue composables (business logic)
│       │   ├── views/         # Page components (route views)
│       │   ├── layouts/       # Layout components (AppLayout)
│       │   ├── router/        # Vue Router configuration
│       │   ├── stores/        # Pinia stores
│       │   ├── services/      # API service layer
│       │   ├── lib/           # Shared utilities (helpers, schemas, request)
│       │   ├── utils/         # Domain-specific utilities (calculators, transformers)
│       │   ├── types/         # TypeScript type definitions
│       │   ├── config/        # App configuration
│       │   ├── assets/        # Static assets (scss, images)
│       │   ├── enums/         # TypeScript enums
│       │   ├── main.ts        # App entry point
│       │   └── App.vue        # Root component
│       ├── vite.config.ts
│       ├── tsconfig.json
│       ├── eslint.config.ts
│       └── package.json
```

## Key Features & Pages

### Routes

- `/home` - HomePage: Dashboard with summary data
- `/per-thread` - Thread-level analysis
- `/sources` - Source code analysis
- `/calltree` - Call tree visualization
- `/timeline` - Timeline analysis
- `/stackPeaks` - Stack memory peaks
- `/allocSizeDistr` - Allocation size distribution
- `/realloc` - Reallocation analysis
- `/globalVars` - Global variables analysis
- `/help` - Help documentation

### Components Organization

Components are organized by domain/feature in subdirectories:

- **`core/`** - Core/layout components used across the application:

  - `AppHeader` - Application header with navigation
  - `BaseModal` - Base modal component for all modals
  - `PageSection` - Generic section wrapper for pages
  - `TooltipWrapper` - Generic tooltip wrapper
  - `WarningTooltip` - Warning tooltip with icon
  - `ExportControls` - Export functionality controls
  - `MetricSelector` - Metric selection dropdown

- **`charts/`** - D3.js chart components:

  - `DonutChart` - Pie/donut charts with legend and click interactions
  - `VerticalBarChart` - Vertical bar charts with tooltips
  - `HorizontalBarChart` - Horizontal bar charts with stacked/grouped modes and grid lines
  - `ScatterChart` - Scatter plot with crosshair on hover and grid lines
  - `Log2HistogramChart` - Log2 histogram with linear scale for size distribution
  - `ReallocGapVerticalBarChart` - Specialized bar chart with sort animation and frequency display
  - `ReallocBundleChart` - Realloc bundle visualization
  - `MostUsedSizesChart` - Most used allocation sizes
  - `PerThreadBarChart` - Per-thread memory usage bars
  - `StackFunctionsBarChart` - Stack functions visualization
  - `StackPeaksBarChart` - Stack peaks chart
  - `StackTimelineChart` - Stack memory over time
  - `TimelineChart` - General timeline visualization
  - `TimeScatterChart` - Time-based scatter plot

- **`home/`** - HomePage-specific components:

  - `InfoCard` - Information card for metrics
  - `StatCard` - Statistics card
  - `SummaryCard` - Summary statistics card
  - `FunctionRankingList` - Ranked function list with progress bars
  - `TopFunctionsList` - Top functions display
  - `HelpModal` - Help modal for HomePage
  - `MetricHelpModal` - Metric-specific help modal
  - `PythonDetailsModal` - Python allocation details modal
  - `SummaryStatsTable` - Summary statistics table

- **`call-tree/`** - CallTree page components:

  - `CallStackTree` - Call stack tree container
  - `CallStackTreeRow` - Individual tree row
  - `CallTreeFilters` - Filtering controls
  - `CallTreeSvgViewer` - SVG visualization viewer

- **`sources/`** - Sources page components:

  - `SourceCodeEditor` - Source code viewer/editor
  - `SourcesSidebar` - Sidebar with file navigation

- **`shared/`** - Shared components used across multiple pages:
  - `AllocatedChunksModal` - Modal for allocated chunks
  - `DomainsTable` - Domains data table
  - `MemoryStatsTable` - Memory statistics table
  - `FunctionList` - Generic function list component

## Code Conventions

### Composables Pattern

The project follows a layered composable architecture:

1. **Data Layer**: `use[Feature]Data.ts` - handles API calls and data fetching
2. **Summary Layer**: `use[Feature]Summary.ts` - computes summary metrics
3. **Charts Layer**: `use[Feature]Charts.ts` - prepares chart data
4. **Composition Layer**: `use[Feature].ts` - combines all layers

Example:

```typescript
export function useGlobalVars() {
  const { data, loading, error } = useGlobalVarsData();
  const { maxThreadCount, globalVariablesMem } = useGlobalVarsSummary(data);
  const { dataForBinaryPie } = useGlobalVarsCharts(data);
  return { data, loading, error, maxThreadCount, dataForBinaryPie };
}
```

### File Naming

- Components: PascalCase (e.g., `DonutChart.vue`, `SummaryCard.vue`)
- Composables: camelCase with `use` prefix (e.g., `useGlobalVars.ts`)
- Services: camelCase with `Service` suffix (e.g., `globalVarsService.ts`)
- Utils: camelCase (e.g., `calculators.ts`, `formatters.ts`, `transformers.ts`)
- Types: kebab-case (e.g., `global-vars.ts`, `realloc.ts`)

### Import Conventions

- **Alias**: `@/` resolves to `src/`
- **Component imports**: Always use absolute paths with `@/components/[folder]/[Component].vue`
  - ✅ Good: `import InfoCard from '@/components/home/InfoCard.vue'`
  - ❌ Bad: `import InfoCard from './InfoCard.vue'` (relative imports)
- **Cross-folder imports**: Use absolute paths to avoid confusion
  - Example: Components in `home/` importing from `core/` use `@/components/core/...`

### Styling

- Primary: Custom SCSS utility classes defined in `src/assets/scss/_utilities.scss`
- SCSS modules and variables in `src/assets/scss/`
- Scoped styles in Vue components using `<style scoped lang="scss">`

### TypeScript

- Strict mode enabled
- Use Zod schemas for runtime validation
- Type definitions in dedicated `types/` directory
- Vue components use `<script setup lang="ts">`

### Linting & Formatting

- **Linter**: ESLint 9 + Oxlint (dual linting setup)
  - Vue plugin: `eslint-plugin-vue`
  - TypeScript config: `@vue/eslint-config-typescript`
  - Skip formatting rules (handled by Prettier)
- **Formatter**: Prettier 3.6
- **CSS Linter**: Stylelint 16 with Vue support
- Run order: `lint:oxlint` → `lint:eslint` → `lint:style`

### State Management

- Use Pinia for global state
- Use composables for feature-specific reactive state
- Pinia plugin: `pinia-plugin-persistedstate` for localStorage persistence

### Data Fetching

- TanStack Query (Vue Query) for server state
- API calls abstracted in service layer (`services/`)
- Request helpers in `lib/request.ts`

## API / Backend Integration

### Development Proxy

Vite dev server proxies API requests to `http://localhost:8080`:

- `/global-variables.json`
- `/data/summary.json`
- `/timed.json`
- `/stacks-mem.json`
- `/flat.json`
- `/scatter.json`
- `/size-map.json`
- `/realloc-map.json`

### Environment Variables

- `VITE_API_URL`: Base API URL (defaults to empty string)

## Development Workflow

### Scripts

```bash
npm run dev              # Start dev server
npm run build            # Type-check + build
npm run preview          # Preview production build
npm run type-check       # Run TypeScript compiler check
npm run lint             # Run all linters (oxlint, eslint, stylelint)
npm run lint:oxlint      # Run oxlint with auto-fix
npm run lint:eslint      # Run ESLint with auto-fix
npm run lint:style       # Run Stylelint
npm run lint:style:fix   # Run Stylelint with auto-fix
npm run format           # Format with Prettier
```

### Git Workflow

- Main branch: `main`
- Current status: Active development on GlobalVars and Realloc pages with new components

## Dependencies to Note

### Key Libraries

- **@tanstack/vue-query**: Server state management
- **d3**: Data visualization
- **@vueuse/core**: Vue composition utilities
- **zod**: Runtime type validation
- **sass-embedded**: SCSS compilation

### Dev Tools

- **vite-plugin-vue-devtools**: Vue DevTools integration
- **oxlint**: Fast Rust-based linter
- **npm-run-all2**: Parallel/sequential script runner

## Common Patterns

### Creating a New Feature Page

1. Create types in `src/types/[feature].ts`
2. Create service in `src/services/[feature]Service.ts`
3. Create layered composables:
   - `src/composables/use[Feature]Data.ts`
   - `src/composables/use[Feature]Summary.ts`
   - `src/composables/use[Feature]Charts.ts`
   - `src/composables/use[Feature].ts`
4. Create utils if needed: `src/utils/[feature]/`
5. Create view: `src/views/[Feature]Page.vue`
6. Add route in `src/router/index.ts`

### Creating a Chart Component

1. Create component in `src/components/charts/[ChartName]Chart.vue`
2. Use D3.js for rendering
3. Accept reactive props with TypeScript types
4. Use composition API with `<script setup>`
5. Apply scoped SCSS for styling
6. Import using absolute path: `@/components/charts/[ChartName]Chart.vue`
7. **Important Chart Conventions**:
   - Use `position: fixed` for tooltips to follow mouse/scroll
   - Remove text-shadow and use normal font-weight for axes text
   - Use black color (`#000` or `black`) for axis text and lines
   - Add grid lines with `#e5e7eb` color and 0.7 opacity
   - Create unique tooltip IDs per component instance to avoid conflicts
   - Clean up tooltips in `onBeforeUnmount()`
   - Use `humanReadable()` helper for formatting byte values
   - Use `Math.log()` for size-based circle radius in scatter plots

### Utility Organization

Domain-specific utilities go in `src/utils/[domain]/`:

- `calculators.ts` - calculations and aggregations
- `formatters.ts` - data formatting
- `transformers.ts` - data transformations

## Notes

- The application analyzes memory profiling data from a backend server
- Charts are built with D3.js for performance and flexibility
- All pages follow lazy-loading pattern for route components
- Legacy Angular code is available in `webview/_client-files_OLD/` for reference

## Chart-Specific Implementation Details

### HorizontalBarChart

- Supports stacked and grouped modes with toggle
- Grid lines for better readability
- Centered tooltips above bars that follow scroll
- Axes with black text (no shadow)

### ReallocGapVerticalBarChart

- Scale band with padding (0.31 inner, 1 outer)
- Animated sorting transition (750ms duration, 50ms delay per bar)
- Auto-enables frequency sort after 500ms
- Special tick formatting: "Others" for 0/Infinity, negative sign for negatives
- Grid lines and "Frequency" Y-axis label

### Log2HistogramChart

- Linear X-scale for continuous range display (min-1 to max+1)
- Bars positioned by `sizeLog` (negative values offset by -1)
- Fixed bar width calculated as `x(1) - x(0)`
- Tooltip shows range (min - max) with frequency
- Tick labels show powers of 2

### ScatterChart

- Circle radius: `Math.max(3, 3 * Math.log(count + 1))`
- Crosshair lines on hover (dashed, #999 color)
- Grid lines on all tick marks
- Opacity 0.6 for overlapping visibility
- Axis labels: "Old Size" (X) and "New Size" (Y)

## Migration Notes (Angular → Vue)

When porting features from Angular (`webview/_client-files_OLD/`):

1. Convert D3 v3 syntax to v7 (scale, axis methods)
2. Replace `d3.tip()` with custom tooltip div
3. Use `d3.scaleBand()` instead of `d3.scale.ordinal().rangeRoundBands()`
4. Use `d3.scaleLinear()` instead of `d3.scale.linear()`
5. Replace NVD3 charts with pure D3 implementations
6. Convert Angular templates to Vue SFC with `<script setup>`

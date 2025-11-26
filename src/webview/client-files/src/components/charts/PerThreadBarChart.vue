<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/charts/PerThreadBarChart.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->

<template>
  <div class="per-thread-bar-chart">
    <!-- Mode toggle -->
    <div class="chart-controls">
      <button
        :class="mode === 'stacked' ? 'btn btn-primary' : 'btn btn-default'"
        @click="mode = 'stacked'"
        type="button"
      >
        Stacked
      </button>
      <button
        :class="mode === 'grouped' ? 'btn btn-primary' : 'btn btn-default'"
        @click="mode = 'grouped'"
        type="button"
      >
        Grouped
      </button>
    </div>

    <svg ref="svgRef" :width="width" :height="chartHeight"></svg>
  </div>
</template>

<script setup lang="ts">
import { ref, computed, watch, onMounted, onBeforeUnmount, nextTick } from 'vue'
import * as d3 from 'd3'
import type { ThreadChartSeries, ChartMetric, ChartMode } from '@/types/per-thread'
import { humanReadable } from '@/lib/helpers'

interface Props {
  data: ThreadChartSeries[]
  metric: ChartMetric
  width?: number | string
  baseHeight?: number
  itemHeight?: number
  ticksPerSecond?: number
}

const props = withDefaults(defineProps<Props>(), {
  width: '100%',
  baseHeight: 150,
  itemHeight: 16,
  ticksPerSecond: 1,
})

const svgRef = ref<SVGSVGElement | null>(null)
const mode = ref<ChartMode>('stacked')

// Margins for the chart
const margin = { top: 30, right: 130, bottom: 50, left: 80 }

/**
 * Calculate dynamic height based on number of threads
 */
const chartHeight = computed(() => {
  if (props.data.length === 0) return props.baseHeight
  const threadCount = props.data[0]?.values.length || 0
  return props.baseHeight + threadCount * props.itemHeight
})

/**
 * Format value based on metric
 */
const formatValue = (value: number): string => {
  switch (props.metric) {
    case 'count':
      return humanReadable(value, 1, '', true)
    case 'time':
      return humanReadable(value / props.ticksPerSecond, 1, 's', true)
    case 'sum':
      return humanReadable(value, 1, 'B', true)
    default:
      return value.toString()
  }
}

/**
 * Color scale for allocation functions
 */
const colorScale = d3.scaleOrdinal(d3.schemeCategory10)

/**
 * Render the chart
 */
const renderChart = () => {
  if (!svgRef.value || !props.data || props.data.length === 0) return

  const svg = d3.select(svgRef.value)
  svg.selectAll('*').remove()

  // Get dimensions
  const containerWidth = svgRef.value.clientWidth || 800
  const width = containerWidth - margin.left - margin.right
  const height = chartHeight.value - margin.top - margin.bottom
  const width_graph = width - 60;

  // Create main group
  const g = svg.append('g').attr('transform', `translate(${margin.left},${margin.top})`)

  // Extract thread names (X-axis categories)
  const threadNames = props.data[0]?.values.map((d) => d.name) || []

  // Calculate max value for X-scale
  let maxValue = 0
  if (mode.value === 'stacked') {
    // For stacked: sum all series values per thread
    threadNames.forEach((_, threadIndex) => {
      const sum = props.data.reduce((acc, series) => acc + series.values[threadIndex].value, 0)
      if (sum > maxValue) maxValue = sum
    })
  } else {
    // For grouped: max of individual values
    maxValue = d3.max(props.data.flatMap((series) => series.values.map((d) => d.value))) || 0
  }

  // Create scales
  const xScale = d3.scaleLinear().domain([0, maxValue]).range([0, width_graph]).nice()

  const yScale = d3.scaleBand().domain(threadNames).range([0, height]).padding(0.2)

  // Create axes
  const xAxis = d3
    .axisBottom(xScale)
    .ticks(10)
    .tickFormat((d) => formatValue(d as number))

  const yAxis = d3.axisLeft(yScale)

  // Add X axis
  g.append('g')
    .attr('transform', `translate(0,${height})`)
    .call(xAxis)
    .selectAll('text')
    .attr('fill', 'black')
    .style('font-weight', 'normal')

  // Add Y axis
  g.append('g')
    .call(yAxis)
    .selectAll('text')
    .attr('fill', 'black')
    .style('font-weight', 'normal')

  // Style axis paths and ticks
  g.selectAll('.domain, .tick line').attr('stroke', 'black')

  // Render bars based on mode
  if (mode.value === 'stacked') {
    renderStackedBars(g, xScale, yScale, threadNames)
  } else {
    renderGroupedBars(g, xScale, yScale, threadNames)
  }

  // Add legend
  const legend = g
    .append('g')
    .attr('class', 'legend')
    .attr('transform', `translate(${width + 10}, 0)`)

  props.data.forEach((series, index) => {
    const legendItem = legend.append('g').attr('transform', `translate(0, ${index * 20})`)

    legendItem
      .append('rect')
      .attr('width', 15)
      .attr('height', 15)
      .attr('fill', colorScale(series.key))

    legendItem
      .append('text')
      .attr('x', 20)
      .attr('y', 12)
      .attr('fill', 'black')
      .style('font-size', '11px')
      .text(series.key)
  })
}

/**
 * Render stacked bars
 */
const renderStackedBars = (
  g: d3.Selection<SVGGElement, unknown, null, undefined>,
  xScale: d3.ScaleLinear<number, number>,
  yScale: d3.ScaleBand<string>,
  threadNames: string[],
) => {
  threadNames.forEach((threadName, threadIndex) => {
    let cumulativeValue = 0

    props.data.forEach((series) => {
      const value = series.values[threadIndex].value
      const barWidth = xScale(value)

      if (value > 0) {
        // Draw bar segment
        g.append('rect')
          .attr('class', 'bar-segment')
          .attr('x', xScale(cumulativeValue))
          .attr('y', yScale(threadName) || 0)
          .attr('width', barWidth)
          .attr('height', yScale.bandwidth())
          .attr('fill', colorScale(series.key))
          .attr('opacity', 0.9)

        // Add value label if significant
        if (barWidth > 30) {
          g.append('text')
            .attr('class', 'bar-label')
            .attr('x', xScale(cumulativeValue) + barWidth / 2)
            .attr('y', (yScale(threadName) || 0) + yScale.bandwidth() / 2)
            .attr('dy', '0.35em')
            .attr('text-anchor', 'middle')
            .attr('fill', 'white')
            .style('font-size', '12px')
            .style('font-weight', 'bold')
            .text(formatValue(value))
        }

        cumulativeValue += value
      }
    })
  })
}

/**
 * Render grouped bars
 */
const renderGroupedBars = (
  g: d3.Selection<SVGGElement, unknown, null, undefined>,
  xScale: d3.ScaleLinear<number, number>,
  yScale: d3.ScaleBand<string>,
  threadNames: string[],
) => {
  const seriesCount = props.data.length
  const groupBandwidth = yScale.bandwidth()
  const barHeight = groupBandwidth / seriesCount

  threadNames.forEach((threadName, threadIndex) => {
    props.data.forEach((series, seriesIndex) => {
      const value = series.values[threadIndex].value
      const barWidth = xScale(value)

      if (value > 0) {
        const yPosition = (yScale(threadName) || 0) + seriesIndex * barHeight

        // Draw bar
        g.append('rect')
          .attr('class', 'bar-segment')
          .attr('x', 0)
          .attr('y', yPosition)
          .attr('width', barWidth)
          .attr('height', barHeight)
          .attr('fill', colorScale(series.key))
          .attr('opacity', 0.9)

        // Add value label if significant
        if (barWidth > 30) {
          g.append('text')
            .attr('class', 'bar-label')
            .attr('x', barWidth + 5)
            .attr('y', yPosition + barHeight / 2)
            .attr('dy', '0.35em')
            .attr('fill', 'black')
            .style('font-size', '12px')
            .text(formatValue(value))
        }
      }
    })
  })
}

// Watch for data or mode changes
watch(
  () => [props.data, mode.value],
  () => {
    nextTick(() => {
      renderChart()
    })
  },
  { deep: true },
)

// Render on mount
onMounted(() => {
  renderChart()
  window.addEventListener('resize', renderChart)
})

// Cleanup
onBeforeUnmount(() => {
  window.removeEventListener('resize', renderChart)
})
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.per-thread-bar-chart {
  width: 100%;
  overflow: auto;
}

.chart-controls {
  display: flex;
  gap: $spacing-sm;
  margin-bottom: $spacing-lg;
  justify-content: flex-end;
}

.btn {
  padding: 6px $spacing-lg;
  border: 1px solid $gray-300;
  border-radius: $radius-sm;
  font-size: $font-size-sm;
  font-weight: $font-weight-medium;
  cursor: pointer;
  transition: all $transition-base;
  font-family: $font-family-base;
}

.btn-default {
  background-color: $color-bg-primary;
  color: $color-text-dark;
}

.btn-default:hover {
  background-color: $gray-100;
}

.btn-primary {
  background-color: $color-primary;
  color: white;
  border-color: $color-primary;
}

.btn-primary:hover {
  background-color: $color-primary-light;
}

svg {
  display: block;
}
</style>

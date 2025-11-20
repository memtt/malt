<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/charts/TimelineChart.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->

<template>
  <div class="timeline-chart-container">
    <svg ref="svgRef" :width="width" :height="height"></svg>
  </div>
</template>

<script setup lang="ts">
import { ref, watch, onMounted, onBeforeUnmount, nextTick } from 'vue'
import * as d3 from 'd3'
import type { ChartSeries } from '@/types/timeline'
import { humanReadable } from '@/lib/helpers'

interface Props {
  data: ChartSeries[]
  yAxisLabel: string
  yAxisUnit: string
  chartType?: 'line' | 'area'
  width?: number | string
  height?: number
  ticksPerSecond?: number
}

const props = withDefaults(defineProps<Props>(), {
  chartType: 'line',
  width: '100%',
  height: 400,
  ticksPerSecond: 1,
})

const emit = defineEmits<{
  pointClick: [timestamp: number]
  dblclick: [timestamp: number]
}>()

const svgRef = ref<SVGSVGElement | null>(null)
const tooltipId = `timeline-tooltip-${Math.random().toString(36).substr(2, 9)}`

// Margins for the chart
const margin = { top: 20, right: 60, bottom: 60, left: 100 }

/**
 * Format time value for X-axis
 */
const formatTime = (ticks: number): string => {
  const seconds = ticks / props.ticksPerSecond

  if (seconds < 60) {
    return `${seconds.toFixed(1)}s`
  } else if (seconds < 3600) {
    const minutes = Math.floor(seconds / 60)
    const secs = Math.floor(seconds % 60)
    return `${minutes}m${secs}s`
  } else {
    const hours = Math.floor(seconds / 3600)
    const minutes = Math.floor((seconds % 3600) / 60)
    return `${hours}h${minutes}m`
  }
}

/**
 * Format value for Y-axis
 */
const formatValue = (value: number): string => {
  return humanReadable(value, 1, props.yAxisUnit, true)
}

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
  const height = props.height - margin.top - margin.bottom

  // Create main group
  const g = svg
    .append('g')
    .attr('transform', `translate(${margin.left},${margin.top})`)

  // Find data extents
  const allValues = props.data.flatMap((series) => series.values)
  const xExtent = d3.extent(allValues, (d) => d.x) as [number, number]
  const yExtent = d3.extent(allValues, (d) => d.y) as [number, number]

  // Create scales
  const xScale = d3.scaleLinear().domain(xExtent).range([0, width]).nice()

  const yScale = d3.scaleLinear().domain([0, yExtent[1]]).range([height, 0]).nice()

  // Create axes
  const xAxis = d3
    .axisBottom(xScale)
    .ticks(10)
    .tickFormat((d) => formatTime(d as number))

  const yAxis = d3
    .axisLeft(yScale)
    .ticks(10)
    .tickFormat((d) => formatValue(d as number))

  // Add grid lines
  g.append('g')
    .attr('class', 'grid')
    .selectAll('line')
    .data(yScale.ticks(10))
    .join('line')
    .attr('x1', 0)
    .attr('x2', width)
    .attr('y1', (d) => yScale(d))
    .attr('y2', (d) => yScale(d))
    .attr('stroke', '#e5e7eb')
    .attr('stroke-opacity', 0.7)

  // Add X grid lines
  g.append('g')
    .attr('class', 'grid')
    .selectAll('line')
    .data(xScale.ticks(10))
    .join('line')
    .attr('x1', (d) => xScale(d))
    .attr('x2', (d) => xScale(d))
    .attr('y1', 0)
    .attr('y2', height)
    .attr('stroke', '#e5e7eb')
    .attr('stroke-opacity', 0.7)

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

  // Add X axis label
  g.append('text')
    .attr('x', width / 2)
    .attr('y', height + 45)
    .attr('text-anchor', 'middle')
    .attr('fill', 'black')
    .style('font-size', '12px')
    .text('Time (seconds)')

  // Add Y axis label
  g.append('text')
    .attr('transform', 'rotate(-90)')
    .attr('x', -height / 2)
    .attr('y', -70)
    .attr('text-anchor', 'middle')
    .attr('fill', 'black')
    .style('font-size', '12px')
    .text(props.yAxisLabel)

  // Create line/area generators
  const lineGenerator = d3
    .line<{ x: number; y: number }>()
    .x((d) => xScale(d.x))
    .y((d) => yScale(d.y))

  const areaGenerator = d3
    .area<{ x: number; y: number }>()
    .x((d) => xScale(d.x))
    .y0(height)
    .y1((d) => yScale(d.y))

  // Draw series (in reverse order so first series is on top)
  props.data
    .slice()
    .reverse()
    .forEach((series) => {
      if (props.chartType === 'area') {
        // Draw area
        g.append('path')
          .datum(series.values)
          .attr('class', 'area')
          .attr('fill', series.color || '#ff7f0e')
          .attr('fill-opacity', 0.6)
          .attr('d', areaGenerator)
      }

      // Draw line (always draw line, even for area charts)
      g.append('path')
        .datum(series.values)
        .attr('class', 'line')
        .attr('fill', 'none')
        .attr('stroke', series.color || '#ff7f0e')
        .attr('stroke-width', 2)
        .attr('d', lineGenerator)
    })

  // Create tooltip
  let tooltip: d3.Selection<HTMLDivElement, unknown, HTMLElement, any> = d3.select(`#${tooltipId}`)
  if (tooltip.empty()) {
    tooltip = (d3
      .select('body')
      .append('div')
      .attr('id', tooltipId)
      .style('position', 'fixed') as d3.Selection<HTMLDivElement, unknown, HTMLElement, any>)
      .style('background-color', 'white')
      .style('border', '1px solid #ccc')
      .style('border-radius', '4px')
      .style('padding', '8px')
      .style('pointer-events', 'none')
      .style('opacity', 0)
      .style('z-index', 1000)
      .style('box-shadow', '0 2px 4px rgba(0,0,0,0.1)')
  }

  // Create guideline
  const guideline = g
    .append('line')
    .attr('class', 'guideline')
    .attr('stroke', '#999')
    .attr('stroke-width', 1)
    .attr('stroke-dasharray', '4,4')
    .attr('opacity', 0)
    .attr('y1', 0)
    .attr('y2', height)

  // Add interaction overlay
  g.append('rect')
    .attr('width', width)
    .attr('height', height)
    .attr('fill', 'none')
    .attr('pointer-events', 'all')
    .on('mousemove', function (event) {
      const [mouseX] = d3.pointer(event)
      const xValue = xScale.invert(mouseX)

      // Show guideline
      guideline.attr('x1', mouseX).attr('x2', mouseX).attr('opacity', 1)

      // Find closest data points for each series
      const tooltipContent = props.data
        .map((series) => {
          // Find closest point
          const bisect = d3.bisector((d: { x: number; y: number }) => d.x).left
          const idx = bisect(series.values, xValue)
          const point =
            idx === 0
              ? series.values[0]
              : idx === series.values.length
                ? series.values[series.values.length - 1]
                : Math.abs(series.values[idx - 1].x - xValue) <
                    Math.abs(series.values[idx].x - xValue)
                  ? series.values[idx - 1]
                  : series.values[idx]

          return `<div style="margin: 2px 0;">
            <span style="color: ${series.color}; font-weight: bold;">${series.key}:</span>
            ${formatValue(point.y)}
          </div>`
        })
        .join('')

      const timeStr = formatTime(xValue)

      tooltip
        .html(`<div style="font-size: 12px;">
          <div style="font-weight: bold; margin-bottom: 4px;">Time: ${timeStr}</div>
          ${tooltipContent}
        </div>`)
        .style('left', `${event.clientX + 10}px`)
        .style('top', `${event.clientY - 10}px`)
        .style('opacity', 1)
    })
    .on('mouseout', () => {
      tooltip.style('opacity', 0)
      guideline.attr('opacity', 0)
    })
    .on('dblclick', function (event) {
      const [mouseX] = d3.pointer(event)
      const xValue = xScale.invert(mouseX)
      emit('dblclick', xValue)
    })
}

// Watch for data changes
watch(
  () => props.data,
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
  d3.select(`#${tooltipId}`).remove()
})

// Expose SVG ref for export
defineExpose({
  svgElement: svgRef,
})
</script>

<style scoped>
.timeline-chart-container {
  width: 100%;
  overflow: auto;
}

svg {
  display: block;
}
</style>

<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/charts/HeatmapChart.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->

<template>
  <div ref="chartContainer" class="heatmap-chart"></div>
</template>

<script setup lang="ts">
import { ref, onMounted, onBeforeUnmount, watch } from 'vue'
import * as d3 from 'd3'
import { humanReadableTimes } from '@/lib/helpers'

export interface HeatmapCell {
  x: number // Bin index X
  y: number // Bin index Y
  value: number // Count/density
  xMin: number // Actual X range min
  xMax: number // Actual X range max
  yMin: number // Actual Y range min
  yMax: number // Actual Y range max
}

interface Props {
  data: HeatmapCell[]
  height?: number
  xLabel?: string
  yLabel?: string
  xUnit?: string
  yUnit?: string
  logX?: boolean
  logY?: boolean
  binsX?: number
  binsY?: number
  logColor?: boolean
  ticksPerSecond?: number
}

const props = withDefaults(defineProps<Props>(), {
  height: 600,
  xLabel: 'X Axis',
  yLabel: 'Y Axis',
  xUnit: '',
  yUnit: '',
  logX: false,
  logY: false,
  binsX: 50,
  binsY: 50,
  logColor: true,
  ticksPerSecond: 1
})

const chartContainer = ref<HTMLDivElement | null>(null)
const componentId = `heatmap-${Math.random().toString(36).substring(2, 11)}`

const renderChart = () => {
  if (!chartContainer.value || !props.data || props.data.length === 0) {
    return
  }

  // Clear previous chart
  d3.select(chartContainer.value).selectAll('*').remove()

  const margin = { top: 20, right: 100, bottom: 80, left: 80 }
  const width = chartContainer.value.clientWidth - margin.left - margin.right
  const height = props.height - margin.top - margin.bottom

  // Create SVG
  const svg = d3
    .select(chartContainer.value)
    .append('svg')
    .attr('width', width + margin.left + margin.right)
    .attr('height', height + margin.top + margin.bottom)
    .append('g')
    .attr('transform', `translate(${margin.left},${margin.top})`)

  // Get unique X and Y bins
  const xBins = Array.from(new Set(props.data.map((d) => d.x))).sort((a, b) => a - b)
  const yBins = Array.from(new Set(props.data.map((d) => d.y))).sort((a, b) => a - b)

  // Create scales for bin positions
  const xScale = d3.scaleBand().domain(xBins.map(String)).range([0, width]).padding(0.01)

  const yScale = d3.scaleBand().domain(yBins.map(String)).range([height, 0]).padding(0.01)

  // Color scale (white to dark blue)
  const nonZeroValues = props.data.filter((d) => d.value > 0).map((d) => d.value)
  const maxValue = d3.max(nonZeroValues) || 1
  const minValue = d3.min(nonZeroValues) || 1

  // Use logarithmic scale for better color distribution if logColor is enabled
  const colorScale = props.logColor
    ? d3.scaleSequential(d3.interpolateTurbo).domain([Math.log(minValue), Math.log(maxValue + 1)])
    : d3.scaleSequential(d3.interpolateTurbo).domain([minValue, maxValue])

  // Draw heatmap cells
  svg
    .selectAll('rect')
    .data(props.data)
    .enter()
    .append('rect')
    .attr('x', (d) => xScale(String(d.x)) || 0)
    .attr('y', (d) => yScale(String(d.y)) || 0)
    .attr('width', xScale.bandwidth())
    .attr('height', yScale.bandwidth())
    .style('fill', (d) => {
      if (d.value === 0) return '#f0f0f0'
      return props.logColor ? colorScale(Math.log(d.value)) : colorScale(d.value)
    })
    .style('stroke', 'none')

  // X Axis
  const xAxisLabels = xBins.filter((_, i) => i % Math.ceil(xBins.length / 10) === 0)
  const xAxis = d3
    .axisBottom(xScale)
    .tickValues(xAxisLabels.map(String))
    .tickFormat((d) => {
      const cell = props.data.find((c) => String(c.x) === d)
      if (!cell) return ''
      const val = (cell.xMin + cell.xMax) / 2
      if (props.xUnit === 'B') return humanReadable(val)
	  if (props.xUnit === 's') return humanReadableTimes(val, props.ticksPerSecond, 1, true)
      return val.toFixed(0)
    })

  svg
    .append('g')
    .attr('transform', `translate(0,${height})`)
    .call(xAxis)
    .selectAll('text')
    .style('text-anchor', 'end')
    .attr('dx', '-.8em')
    .attr('dy', '.15em')
    .attr('transform', 'rotate(-45)')
    .style('font-size', '11px')
    .style('fill', 'black')

  // Y Axis
  const yAxisLabels = yBins.filter((_, i) => i % Math.ceil(yBins.length / 10) === 0)
  const yAxis = d3
    .axisLeft(yScale)
    .tickValues(yAxisLabels.map(String))
    .tickFormat((d) => {
      const cell = props.data.find((c) => String(c.y) === d)
      if (!cell) return ''
      const val = (cell.yMin + cell.yMax) / 2
	  if (props.yUnit === 'B') return humanReadable(val)
	  if (props.yUnit === 's') return humanReadableTimes(val, props.ticksPerSecond, 1, true)
      return val.toFixed(0)
    })

  svg.append('g').call(yAxis).selectAll('text').style('font-size', '11px').style('fill', 'black')

  // X Axis Label
  svg
    .append('text')
    .attr('x', width / 2)
    .attr('y', height + margin.bottom - 10)
    .style('text-anchor', 'middle')
    .style('font-size', '14px')
    .style('fill', 'black')
    .text(props.xLabel)

  // Y Axis Label
  svg
    .append('text')
    .attr('transform', 'rotate(-90)')
    .attr('x', -height / 2)
    .attr('y', -margin.left + 20)
    .style('text-anchor', 'middle')
    .style('font-size', '14px')
    .style('fill', 'black')
    .text(props.yLabel)

  // Legend
  const legendWidth = 20
  const legendHeight = height

  const legendScale = props.logColor
    ? d3
        .scaleLinear()
        .domain([Math.log(minValue), Math.log(maxValue + 1)])
        .range([legendHeight, 0])
    : d3.scaleLinear().domain([minValue, maxValue]).range([legendHeight, 0])

  const legendAxis = d3
    .axisRight(legendScale)
    .ticks(5)
    .tickFormat((d) => {
      const value = props.logColor ? Math.exp(d as number) : (d as number)
      return Math.round(value).toLocaleString()
    })

  const legendSvg = svg.append('g').attr('transform', `translate(${width + 20}, 0)`)

  // Create gradient
  const defs = svg.append('defs')
  const gradient = defs
    .append('linearGradient')
    .attr('id', `heatmap-gradient-${componentId}`)
    .attr('x1', '0%')
    .attr('y1', '100%')
    .attr('x2', '0%')
    .attr('y2', '0%')

  gradient
    .selectAll('stop')
    .data(
      d3.range(0, 1.01, 0.1).map((t) => {
        const value = props.logColor
          ? Math.log(minValue) + t * (Math.log(maxValue + 1) - Math.log(minValue))
          : minValue + t * (maxValue - minValue)
        return {
          offset: `${t * 100}%`,
          color: colorScale(value),
        }
      }),
    )
    .enter()
    .append('stop')
    .attr('offset', (d) => d.offset)
    .attr('stop-color', (d) => d.color)

  legendSvg
    .append('rect')
    .attr('width', legendWidth)
    .attr('height', legendHeight)
    .style('fill', `url(#heatmap-gradient-${componentId})`)

  legendSvg.append('g').attr('transform', `translate(${legendWidth}, 0)`).call(legendAxis)
}

const humanReadable = (bytes: number): string => {
  if (bytes === 0) return '0 B'
  const k = 1024
  const sizes = ['B', 'KB', 'MB', 'GB', 'TB']
  const i = Math.floor(Math.log(bytes) / Math.log(k))
  return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i]
}

onMounted(() => {
  renderChart()
  window.addEventListener('resize', renderChart)
})

onBeforeUnmount(() => {
  window.removeEventListener('resize', renderChart)
})

watch(() => props.data, renderChart, { deep: true })
</script>

<style scoped>
.heatmap-chart {
  width: 100%;
  min-height: 400px;
  overflow: auto;
}
</style>

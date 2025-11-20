<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/charts/TimeScatterChart.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->
<template>
  <div class="time-scatter-chart-wrapper">
    <div ref="chartContainer" class="time-scatter-chart"></div>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted, onBeforeUnmount, watch } from 'vue'
import * as d3 from 'd3'
import { humanReadable } from '@/lib/helpers'
import type { ScatterSeries } from '@/types/alloc-size-distr'

interface Props {
  data: ScatterSeries
  height?: number
  xAxisLabel?: string
  yAxisLabel?: string
  xUnit?: string
  yUnit?: string
  logX?: boolean
  logY?: boolean
}

const props = withDefaults(defineProps<Props>(), {
  height: 500,
  xAxisLabel: 'X Axis',
  yAxisLabel: 'Y Axis',
  xUnit: '',
  yUnit: '',
  logX: false,
  logY: false,
})

const chartContainer = ref<HTMLElement | null>(null)

// Create unique tooltip ID for this component instance
const tooltipId = `time-scatter-tooltip-${Math.random().toString(36).substr(2, 9)}`

// Create tooltip once and reuse it
let tooltip: d3.Selection<HTMLDivElement, unknown, HTMLElement, any> | null = null

const getOrCreateTooltip = () => {
  if (!tooltip || tooltip.empty()) {
    d3.selectAll(`.${tooltipId}`).remove()

    tooltip = d3
      .select('body')
      .append('div')
      .attr('class', `time-scatter-tooltip ${tooltipId}`)
      .style('position', 'fixed')
      .style('background', 'white')
      .style('padding', '12px')
      .style('border', '1px solid #ddd')
      .style('border-radius', '6px')
      .style('pointer-events', 'none')
      .style('opacity', 0)
      .style('box-shadow', '0 4px 6px rgba(0,0,0,0.1)')
      .style('z-index', '9999')
      .style('max-width', '300px')
      .style('font-size', '13px')
  }
  return tooltip
}

const updateTooltipPosition = (event: MouseEvent) => {
  if (!tooltip) return

  const tooltipNode = tooltip.node()
  if (tooltipNode) {
    tooltip.style('left', `${event.clientX + 10}px`).style('top', `${event.clientY - 28}px`)
  }
}

const buildChart = () => {
  if (!chartContainer.value || !props.data || props.data.points.length === 0) return

  d3.select(chartContainer.value).selectAll('*').remove()

  const margin = { top: 20, right: 30, bottom: 60, left: 80 }
  const width = chartContainer.value.clientWidth || 800
  const height = props.height - margin.top - margin.bottom

  const svg = d3
    .select(chartContainer.value)
    .append('svg')
    .attr('width', width)
    .attr('height', props.height)
    .append('g')
    .attr('transform', `translate(${margin.left},${margin.top})`)

  const tooltipElement = getOrCreateTooltip()

  // Transform data for log scale if needed
  const transformedData = props.data.points.map((d) => ({
    x: props.logX ? Math.log2(d.x) : d.x,
    y: props.logY ? Math.log2(d.y) : d.y,
    v: d.v,
    originalX: d.x,
    originalY: d.y,
  }))

  // Scales
  const xMin = d3.min(transformedData, (d) => d.x) || 0
  const xMax = d3.max(transformedData, (d) => d.x) || 0
  const yMin = d3.min(transformedData, (d) => d.y) || 0
  const yMax = d3.max(transformedData, (d) => d.y) || 0

  const x = d3
    .scaleLinear()
    .domain([xMin, xMax])
    .range([0, width - margin.left - margin.right])

  const y = d3.scaleLinear().domain([yMin, yMax]).range([height, 0])

  // Add grid lines
  svg
    .append('g')
    .attr('class', 'grid-minor')
    .selectAll('line.x-grid')
    .data(x.ticks())
    .enter()
    .append('line')
    .attr('class', 'x-grid')
    .attr('x1', (d) => x(d))
    .attr('x2', (d) => x(d))
    .attr('y1', 0)
    .attr('y2', height)
    .style('stroke', '#f0f0f0')
    .style('stroke-width', 1)
    .style('stroke-opacity', 0.5)

  svg
    .append('g')
    .attr('class', 'grid-minor')
    .selectAll('line.y-grid')
    .data(y.ticks())
    .enter()
    .append('line')
    .attr('class', 'y-grid')
    .attr('x1', 0)
    .attr('x2', width - margin.left - margin.right)
    .attr('y1', (d) => y(d))
    .attr('y2', (d) => y(d))
    .style('stroke', '#f0f0f0')
    .style('stroke-width', 1)
    .style('stroke-opacity', 0.5)

  // Create crosshair group
  const crosshairGroup = svg.append('g').attr('class', 'crosshair').style('display', 'none')

  const crosshairX = crosshairGroup
    .append('line')
    .attr('class', 'crosshair-x')
    .attr('y1', 0)
    .style('stroke', '#999')
    .style('stroke-width', 1)
    .style('stroke-dasharray', '3,3')

  const crosshairY = crosshairGroup
    .append('line')
    .attr('class', 'crosshair-y')
    .attr('x1', 0)
    .style('stroke', '#999')
    .style('stroke-width', 1)
    .style('stroke-dasharray', '3,3')

  // Add circles with varying sizes based on count
  const maxCount = d3.max(transformedData, (d) => d.v) || 1

  svg
    .selectAll('circle')
    .data(transformedData)
    .enter()
    .append('circle')
    .attr('cx', (d) => x(d.x))
    .attr('cy', (d) => y(d.y))
    .attr('r', (d) => {
      // Size proportional to count, with min/max bounds
      const normalized = d.v / maxCount
      return 3 + normalized * 15 // Range from 3px to 18px
    })
    .attr('fill', 'steelblue')
    .attr('opacity', 0.6)
    .on('mouseover', function (event: MouseEvent, d) {
      const cx = x(d.x)
      const cy = y(d.y)

      // Show crosshair
      crosshairGroup.style('display', null)
      crosshairX.attr('x1', cx).attr('x2', cx).attr('y2', height)
      crosshairY
        .attr('y1', cy)
        .attr('y2', cy)
        .attr('x2', width - margin.left - margin.right)

      d3.select(this).attr('fill', 'orangered')
      tooltipElement.transition().duration(200).style('opacity', 0.9)

      const xValue = props.logX
        ? humanReadable(d.originalX, 1, props.xUnit, true)
        : d.originalX.toString()
      const yValue = props.logY
        ? humanReadable(d.originalY, 1, props.yUnit, true)
        : d.originalY.toString()

      tooltipElement.html(
        `<div><strong>${props.xAxisLabel}:</strong> ${xValue}</div>
         <div><strong>${props.yAxisLabel}:</strong> ${yValue}</div>
         <div><strong>Count:</strong> ${d.v.toLocaleString()}</div>`,
      )

      updateTooltipPosition(event)
    })
    .on('mousemove', function (event: MouseEvent) {
      updateTooltipPosition(event)
    })
    .on('mouseout', function () {
      // Hide crosshair
      crosshairGroup.style('display', 'none')

      d3.select(this).attr('fill', 'steelblue')
      tooltipElement.transition().duration(500).style('opacity', 0)
    })

  // X axis
  const xAxis = svg
    .append('g')
    .attr('class', 'x axis')
    .attr('transform', `translate(0,${height})`)
    .call(
      d3.axisBottom(x).tickFormat((d) => {
        const val = Number(d)
        return props.logX ? humanReadable(Math.pow(2, val), 1, props.xUnit, false) : val.toString()
      }),
    )

  xAxis
    .selectAll('text')
    .style('fill', 'black')
    .style('text-shadow', 'none')
    .style('font-weight', 'normal')
    .attr('dy', '1.5em')

  xAxis.select('.domain').style('stroke', 'black')
  xAxis.selectAll('.tick line').style('stroke', 'black')

  // Y axis
  const yAxis = svg
    .append('g')
    .attr('class', 'y axis')
    .call(
      d3.axisLeft(y).tickFormat((d) => {
        const val = Number(d)
        return props.logY ? humanReadable(Math.pow(2, val), 1, props.yUnit, false) : val.toString()
      }),
    )

  yAxis
    .selectAll('text')
    .style('fill', 'black')
    .style('text-shadow', 'none')
    .style('font-weight', 'normal')
    .attr('dx', '-0.5em')

  yAxis.select('.domain').style('stroke', 'black')
  yAxis.selectAll('.tick line').style('stroke', 'black')

  // X axis label
  svg
    .append('text')
    .attr('x', (width - margin.left - margin.right) / 2)
    .attr('y', height + 50)
    .attr('text-anchor', 'middle')
    .style('font-size', '14px')
    .style('fill', 'black')
    .text(props.xAxisLabel)

  // Y axis label
  svg
    .append('text')
    .attr('transform', 'rotate(-90)')
    .attr('x', -height / 2)
    .attr('y', -60)
    .attr('text-anchor', 'middle')
    .style('font-size', '14px')
    .style('fill', 'black')
    .text(props.yAxisLabel)

  // Add legend
  const legend = svg
    .append('g')
    .attr('class', 'legend')
    .attr('transform', `translate(${width - margin.left - margin.right - 60}, -5)`)

  legend
    .append('circle')
    .attr('cx', 0)
    .attr('cy', 0)
    .attr('r', 5)
    .attr('fill', 'steelblue')
    .attr('opacity', 0.6)

  legend
    .append('text')
    .attr('x', 10)
    .attr('y', 0)
    .attr('dy', '0.35em')
    .style('font-size', '12px')
    .style('fill', 'black')
    .text('Alloc')
}

onMounted(() => {
  buildChart()
})

onBeforeUnmount(() => {
  d3.selectAll(`.${tooltipId}`).remove()
})

watch(() => props.data, buildChart, { deep: true })
</script>

<style scoped>
.time-scatter-chart-wrapper {
  width: 100%;
  overflow: auto;
}

.time-scatter-chart {
  position: relative;
  width: 100%;
  min-height: 500px;
}
</style>

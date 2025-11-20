<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/charts/ScatterChart.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->

<template>
  <div class="scatter-chart-wrapper">
    <div ref="chartContainer" class="scatter-chart"></div>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted, onBeforeUnmount, watch } from 'vue'
import * as d3 from 'd3'
import { humanReadable } from '@/lib/helpers'

interface ScatterData {
  oldSize: number
  newSize: number
  count: number
}

interface Props {
  data: ScatterData[]
  height?: number
}

const props = withDefaults(defineProps<Props>(), {
  height: 500,
})

const chartContainer = ref<HTMLElement | null>(null)

// Create unique tooltip ID for this component instance
const tooltipId = `scatter-chart-tooltip-${Math.random().toString(36).substr(2, 9)}`

// Create tooltip once and reuse it
let tooltip: d3.Selection<HTMLDivElement, unknown, HTMLElement, any> | null = null
let currentCircle: SVGCircleElement | null = null

const getOrCreateTooltip = () => {
  if (!tooltip || tooltip.empty()) {
    d3.selectAll(`.${tooltipId}`).remove()

    tooltip = d3
      .select('body')
      .append('div')
      .attr('class', `scatter-chart-tooltip ${tooltipId}`)
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
  if (!chartContainer.value || props.data.length === 0) return

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

  // Scales - Note: x is newSize, y is oldSize (matching Angular)
  const xMax = d3.max(props.data, (d) => d.newSize) || 0
  const yMax = d3.max(props.data, (d) => d.oldSize) || 0

  const x = d3
    .scaleLinear()
    .domain([0, xMax])
    .range([0, width - margin.left - margin.right])

  const y = d3.scaleLinear().domain([0, yMax]).range([height, 0])

  // Add grid lines (use default tick count ~10)
  svg
    .append('g')
    .attr('class', 'grid')
    .selectAll('line.x-grid')
    .data(x.ticks(10))
    .enter()
    .append('line')
    .attr('class', 'x-grid')
    .attr('x1', (d) => x(d))
    .attr('x2', (d) => x(d))
    .attr('y1', 0)
    .attr('y2', height)
    .style('stroke', '#e5e7eb')
    .style('stroke-width', 1)
    .style('opacity', 0.7)

  svg
    .append('g')
    .attr('class', 'grid')
    .selectAll('line.y-grid')
    .data(y.ticks(10))
    .enter()
    .append('line')
    .attr('class', 'y-grid')
    .attr('x1', 0)
    .attr('x2', width - margin.left - margin.right)
    .attr('y1', (d) => y(d))
    .attr('y2', (d) => y(d))
    .style('stroke', '#e5e7eb')
    .style('stroke-width', 1)
    .style('opacity', 0.7)

  // Create group for crosshair lines
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

  // Add circles
  svg
    .selectAll('circle')
    .data(props.data)
    .enter()
    .append('circle')
    .attr('cx', (d) => x(d.newSize))
    .attr('cy', (d) => y(d.oldSize))
    .attr('r', (d) => Math.max(2, 2 * Math.log(d.count + 1)))
    .attr('fill', 'steelblue')
    .attr('opacity', 0.6)
    .on('mouseover', function (event: MouseEvent, d) {
      const circle = this as SVGCircleElement
      currentCircle = circle

      const cx = x(d.newSize)
      const cy = y(d.oldSize)

      // Show crosshair
      crosshairGroup.style('display', null)
      crosshairX.attr('x1', cx).attr('x2', cx).attr('y2', height)
      crosshairY
        .attr('y1', cy)
        .attr('y2', cy)
        .attr('x2', width - margin.left - margin.right)

      d3.select(circle).attr('fill', 'orangered')
      tooltipElement.transition().duration(200).style('opacity', 0.9)

      tooltipElement.html(
        `<div style="margin-bottom: 4px;"><strong>${humanReadable(d.newSize, 1, 'B', true)}</strong></div>
         <div style="display: flex; align-items: center; gap: 6px;">
           <div style="width: 12px; height: 12px; background: steelblue; border-radius: 2px;"></div>
           <span>Realloc <strong>${humanReadable(d.oldSize, 1, 'B', true)}</strong></span>
         </div>`,
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
      currentCircle = null
    })

  // X axis
  const xAxis = svg
    .append('g')
    .attr('class', 'x axis')
    .attr('transform', `translate(0,${height})`)
    .call(
      d3
        .axisBottom(x)
        .ticks(10)
        .tickFormat((d) => humanReadable(d as number, 1, 'B', true)),
    )

  xAxis
    .selectAll('text')
    .style('fill', 'black')
    .style('text-shadow', 'none')
    .style('font-weight', 'normal')

  xAxis.selectAll('.tick line').style('stroke', 'black')

  xAxis.select('.domain').style('stroke', 'black')

  // Y axis
  const yAxis = svg
    .append('g')
    .attr('class', 'y axis')
    .call(
      d3
        .axisLeft(y)
        .ticks(10)
        .tickFormat((d) => humanReadable(d as number, 1, 'B', true)),
    )

  yAxis
    .selectAll('text')
    .style('fill', 'black')
    .style('text-shadow', 'none')
    .style('font-weight', 'normal')

  yAxis.selectAll('.tick line').style('stroke', 'black')

  yAxis.select('.domain').style('stroke', 'black')

  // X axis label (displays newSize data, but labeled as "Old size" to match Angular)
  svg
    .append('text')
    .attr('x', (width - margin.left - margin.right) / 2)
    .attr('y', height + 50)
    .attr('text-anchor', 'middle')
    .style('font-size', '14px')
    .style('fill', 'black')
    .text('Old size')

  // Y axis label (displays oldSize data, but labeled as "New size" to match Angular)
  svg
    .append('text')
    .attr('transform', 'rotate(-90)')
    .attr('x', -height / 2)
    .attr('y', -60)
    .attr('text-anchor', 'middle')
    .style('font-size', '14px')
    .style('fill', 'black')
    .text('New size')

  // Add legend
  const legend = svg
    .append('g')
    .attr('class', 'legend')
    .attr('transform', `translate(${width - margin.left - margin.right - 80}, -15)`)

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
    .text('Realloc')
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
.scatter-chart-wrapper {
  width: 100%;
  overflow: auto;
}

.scatter-chart {
  position: relative;
  width: 100%;
  min-height: 500px;
}
</style>

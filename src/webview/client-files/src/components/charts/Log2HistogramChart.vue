<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/charts/Log2HistogramChart.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->

<template>
  <div class="log2-histogram-wrapper">
    <div ref="chartContainer" class="log2-histogram-chart"></div>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted, onBeforeUnmount, watch } from 'vue'
import * as d3 from 'd3'
import { humanReadable } from '@/lib/helpers'

interface Log2Data {
  sizeLog: number
  count: number
  label?: string
}

interface Props {
  data: Log2Data[]
  height?: number
}

const props = withDefaults(defineProps<Props>(), {
  height: 500,
})

const chartContainer = ref<HTMLElement | null>(null)

// Create unique tooltip ID for this component instance
const tooltipId = `log2-histogram-tooltip-${Math.random().toString(36).substr(2, 9)}`

// Create tooltip once and reuse it
let tooltip: d3.Selection<HTMLDivElement, unknown, HTMLElement, any> | null = null
let currentBar: SVGRectElement | null = null

const getOrCreateTooltip = () => {
  if (!tooltip || tooltip.empty()) {
    d3.selectAll(`.${tooltipId}`).remove()

    tooltip = d3
      .select('body')
      .append('div')
      .attr('class', `log2-histogram-tooltip ${tooltipId}`)
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
      .style('transform', 'translateX(-50%)')
  }
  return tooltip
}

const updateTooltipPosition = () => {
  if (!currentBar || !tooltip) return

  const barRect = currentBar.getBoundingClientRect()
  const tooltipNode = tooltip.node()

  if (tooltipNode) {
    const barCenterX = barRect.left + barRect.width / 2
    const topY = barRect.top - tooltipNode.offsetHeight - 10

    tooltip.style('left', `${barCenterX}px`).style('top', `${topY}px`)
  }
}

const handleScroll = () => {
  if (currentBar) {
    updateTooltipPosition()
  }
}

const buildChart = () => {
  if (!chartContainer.value || props.data.length === 0) return

  d3.select(chartContainer.value).selectAll('*').remove()

  const margin = { top: 20, right: 20, bottom: 60, left: 80 }
  const width = chartContainer.value.clientWidth || 960
  const height = props.height - margin.top - margin.bottom

  const svg = d3
    .select(chartContainer.value)
    .append('svg')
    .attr('width', width)
    .attr('height', props.height)
    .append('g')
    .attr('transform', `translate(${margin.left},${margin.top})`)

  const tooltipElement = getOrCreateTooltip()

  // Create linear scale for x-axis
  const minSizeLog = d3.min(props.data, (d) => d.sizeLog) || 0
  const maxSizeLog = d3.max(props.data, (d) => d.sizeLog) || 0

  const x = d3
    .scaleLinear()
    .domain([minSizeLog - 1, maxSizeLog + 1])
    .range([0, width - margin.left - margin.right])

  const y = d3
    .scaleLinear()
    .domain([0, d3.max(props.data, (d) => d.count) || 0])
    .range([height, 0])

  // Add grid lines
  svg
    .append('g')
    .attr('class', 'grid')
    .call(
      d3
        .axisLeft(y)
        .tickSize(-(width - margin.left - margin.right))
        .tickFormat(() => ''),
    )
    .selectAll('line')
    .style('stroke', '#e5e7eb')
    .style('stroke-opacity', 0.7)

  svg.selectAll('.grid .domain').remove()

  // Calculate bar width
  const barWidth = x(1) - x(0)

  // Add bars
  svg
    .selectAll('.bar')
    .data(props.data)
    .enter()
    .append('rect')
    .attr('class', 'bar')
    .attr('x', (d) => (d.sizeLog >= 0 ? x(d.sizeLog) : x(d.sizeLog - 1)))
    .attr('width', barWidth)
    .attr('y', (d) => y(d.count))
    .attr('height', (d) => height - y(d.count))
    .attr('fill', 'steelblue')
    .on('mouseover', function (event, d) {
      const bar = this as SVGRectElement
      currentBar = bar

      d3.select(bar).attr('fill', 'orangered')
      tooltipElement.transition().duration(200).style('opacity', 0.9)

      const isNegative = d.sizeLog < 0
      const absSizeLog = Math.abs(d.sizeLog)
      const min = Math.pow(2, absSizeLog - 1)
      const max = Math.pow(2, absSizeLog)

      const minLabel = humanReadable(isNegative ? -max : min, 1, 'B', true)
      const maxLabel = humanReadable(isNegative ? -min : max, 1, 'B', true)

      tooltipElement.html(
        `<strong>${minLabel} - ${maxLabel}:</strong><br/>Frequency: <span style="color: red; font-weight: bold;">${humanReadable(d.count, 1, '', true)}</span>`,
      )

      updateTooltipPosition()
    })
    .on('mouseout', function () {
      d3.select(this).attr('fill', 'steelblue')
      tooltipElement.transition().duration(500).style('opacity', 0)
      currentBar = null
    })

  // X axis
  svg
    .append('g')
    .attr('class', 'x axis')
    .attr('transform', `translate(0,${height})`)
    .call(
      d3.axisBottom(x).tickFormat((d) => {
        const val = d as number
        if (val === 0) return '0 B'
        const sign = val < 0 ? '-' : ''
        const absVal = Math.abs(val)
        return sign + humanReadable(Math.pow(2, absVal - 1), 1, 'B', true)
      }),
    )
    .selectAll('text')
    .style('fill', 'black')
    .style('text-shadow', 'none')
    .style('font-weight', 'normal')

  // Y axis
  const yAxis = svg
    .append('g')
    .attr('class', 'y axis')
    .call(d3.axisLeft(y).tickFormat((d) => humanReadable(d as number, 1, '', true)))
  yAxis
    .selectAll('text')
    .style('fill', 'black')
    .style('text-shadow', 'none')
    .style('font-weight', 'normal')
  yAxis.selectAll('line').style('stroke', 'black')
  yAxis.select('.domain').style('stroke', 'black')

  // Y axis label
  svg
    .append('text')
    .attr('transform', 'rotate(-90)')
    .attr('x', -height / 2)
    .attr('y', -60)
    .attr('text-anchor', 'middle')
    .style('font-size', '14px')
    .style('fill', 'black')
    .text('Frequency')
}

onMounted(() => {
  buildChart()
  window.addEventListener('scroll', handleScroll, true)
})

onBeforeUnmount(() => {
  d3.selectAll(`.${tooltipId}`).remove()
  window.removeEventListener('scroll', handleScroll, true)
})

watch(() => props.data, buildChart, { deep: true })
</script>

<style scoped>
.log2-histogram-wrapper {
  width: 100%;
  overflow: auto;
}

.log2-histogram-chart {
  position: relative;
  width: 100%;
  min-height: 500px;
}
</style>

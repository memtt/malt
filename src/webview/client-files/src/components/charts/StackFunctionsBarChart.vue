<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/charts/StackFunctionsBarChart.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->

<template>
  <div class="stack-functions-bar-chart">
    <svg ref="svgRef" :width="width" :height="chartHeight"></svg>
  </div>
</template>

<script setup lang="ts">
import { ref, computed, watch, onMounted, onBeforeUnmount, nextTick } from 'vue'
import * as d3 from 'd3'
import type { BarChartData } from '@/types/stack-peaks'
import { humanReadable } from '@/lib/helpers'

interface Props {
  data: BarChartData[]
  width?: number | string
  baseHeight?: number
  itemHeight?: number
}

const props = withDefaults(defineProps<Props>(), {
  width: '100%',
  baseHeight: 150,
  itemHeight: 16,
})

const svgRef = ref<SVGSVGElement | null>(null)

// Margins for the chart
const margin = { top: 30, right: 100, bottom: 50, left: 200 }

/**
 * Calculate dynamic height based on number of items
 */
const chartHeight = computed(() => {
  return props.baseHeight + props.data.length * props.itemHeight
})

/**
 * Format value for display
 */
const formatValue = (value: number): string => {
  return humanReadable(value, 1, 'B', true)
}

/**
 * Truncate long function names for Y-axis labels
 */
const truncateLabel = (label: string, maxLength: number = 30): string => {
  if (label.length <= maxLength) return label
  return label.substring(0, maxLength - 3) + '...'
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
  const height = chartHeight.value - margin.top - margin.bottom

  // Create main group
  const g = svg.append('g').attr('transform', `translate(${margin.left},${margin.top})`)

  // Create scales
  const xScale = d3
    .scaleLinear()
    .domain([0, d3.max(props.data, (d) => d.value) || 0])
    .range([0, width])
    .nice()

  const yScale = d3
    .scaleBand()
    .domain(props.data.map((d) => d.label))
    .range([0, height])
    .padding(0.2)

  // Create axes
  const xAxis = d3
    .axisBottom(xScale)
    .ticks(10)
    .tickFormat((d) => formatValue(d as number))

  const yAxis = d3.axisLeft(yScale).tickFormat((d) => truncateLabel(d as string))

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
    .style('font-size', '10px')

  // Style axis paths and ticks
  g.selectAll('.domain, .tick line').attr('stroke', 'black')

  // Add X axis label
  g.append('text')
    .attr('x', width / 2)
    .attr('y', height + 45)
    .attr('text-anchor', 'middle')
    .attr('fill', 'black')
    .style('font-size', '12px')
    .text('Stack memory')

  // Add bars
  g.selectAll('.bar')
    .data(props.data)
    .join('rect')
    .attr('class', 'bar')
    .attr('x', 0)
    .attr('y', (d) => yScale(d.label) || 0)
    .attr('width', (d) => xScale(d.value))
    .attr('height', yScale.bandwidth())
    .attr('fill', '#2563eb')

  // Add value labels on bars
  g.selectAll('.bar-label')
    .data(props.data)
    .join('text')
    .attr('class', 'bar-label')
    .attr('x', (d) => xScale(d.value) + 5)
    .attr('y', (d) => (yScale(d.label) || 0) + yScale.bandwidth() / 2)
    .attr('dy', '0.35em')
    .attr('fill', 'black')
    .style('font-size', '11px')
    .style('font-weight', 'normal')
    .text((d) => formatValue(d.value))

  // Add tooltips for full function names on hover
  const tooltip = d3
    .select('body')
    .append('div')
    .attr('class', 'function-tooltip')
    .style('position', 'fixed')
    .style('background-color', 'white')
    .style('border', '1px solid #ccc')
    .style('border-radius', '4px')
    .style('padding', '8px')
    .style('pointer-events', 'none')
    .style('opacity', 0)
    .style('z-index', 1000)
    .style('box-shadow', '0 2px 4px rgba(0,0,0,0.1)')
    .style('max-width', '400px')
    .style('word-wrap', 'break-word')

  // Add hover interactions to Y-axis labels
  g.selectAll('.tick text').on('mouseenter', function (event, d) {
    const barData = props.data.find((item) => item.label === d)
    if (barData) {
      let tooltipContent = `<div style="font-size: 12px;">
        <div style="font-weight: bold; margin-bottom: 4px;">${barData.label}</div>
        <div>Memory: ${formatValue(barData.value)}</div>`

      if (barData.info?.file) {
        tooltipContent += `<div style="margin-top: 4px; color: #6b7280; font-size: 11px;">
          ${barData.info.file}${barData.info.line ? ':' + barData.info.line : ''}
        </div>`
      }

      tooltipContent += '</div>'

      tooltip
        .html(tooltipContent)
        .style('left', `${event.clientX + 10}px`)
        .style('top', `${event.clientY - 10}px`)
        .style('opacity', 1)
    }
  })

  g.selectAll('.tick text').on('mouseleave', function () {
    tooltip.style('opacity', 0)
  })

  // Cleanup tooltip on chart destruction
  svg.on('remove', () => {
    tooltip.remove()
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
  d3.selectAll('.function-tooltip').remove()
})
</script>

<style scoped>
.stack-functions-bar-chart {
  width: 100%;
  overflow: auto;
}

svg {
  display: block;
}
</style>

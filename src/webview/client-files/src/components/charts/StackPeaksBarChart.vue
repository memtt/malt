<template>
  <div class="stack-peaks-bar-chart">
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
  selectedIndex?: number | null
}

const props = withDefaults(defineProps<Props>(), {
  width: '100%',
  baseHeight: 150,
  itemHeight: 16,
  selectedIndex: null,
})

const emit = defineEmits<{
  barClick: [index: number]
}>()

const svgRef = ref<SVGSVGElement | null>(null)

// Margins for the chart
const margin = { top: 30, right: 100, bottom: 50, left: 40 }

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

  // Add X axis label
  g.append('text')
    .attr('x', width / 2)
    .attr('y', height + 45)
    .attr('text-anchor', 'middle')
    .attr('fill', 'black')
    .style('font-size', '12px')
    .text('Maximum stack size')

  // Add Y axis label
  g.append('text')
    .attr('transform', 'rotate(-90)')
    .attr('x', -height / 2)
    .attr('y', -30)
    .attr('text-anchor', 'middle')
    .attr('fill', 'black')
    .style('font-size', '12px')
    .text('Thread ID')

  // Add bars
  const bars = g
    .selectAll('.bar')
    .data(props.data)
    .join('rect')
    .attr('class', 'bar')
    .attr('x', 0)
    .attr('y', (d) => yScale(d.label) || 0)
    .attr('width', (d) => xScale(d.value))
    .attr('height', yScale.bandwidth())
    .attr('fill', (d) => (d.index === props.selectedIndex ? '#1d4ed8' : '#2563eb'))
    .attr('cursor', 'pointer')
    .on('click', function (event, d) {
      emit('barClick', d.index)
    })
    .on('mouseenter', function () {
      d3.select(this).attr('opacity', 0.8)
    })
    .on('mouseleave', function () {
      d3.select(this).attr('opacity', 1)
    })

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
}

// Watch for data changes
watch(
  () => [props.data, props.selectedIndex],
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

<style scoped>
.stack-peaks-bar-chart {
  width: 100%;
  overflow: auto;
}

svg {
  display: block;
}
</style>

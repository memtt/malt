<template>
  <div class="realloc-gap-chart-wrapper">
    <!-- Controls -->
    <div v-if="enableSort" class="chart-controls">
      <label class="checkbox-label">
        <input type="checkbox" v-model="sortByFrequency" />
        <span>{{ sortLabel }}</span>
      </label>
    </div>

    <!-- Chart Container -->
    <div ref="chartContainer" class="realloc-gap-chart"></div>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted, onBeforeUnmount, watch } from 'vue'
import * as d3 from 'd3'
import { humanReadable } from '@/lib/helpers'

interface DeltaData {
  delta?: number
  label?: string
  count: number
}

interface Props {
  data: DeltaData[]
  height?: number
  sortLabel?: string
  enableSort?: boolean
}

const props = withDefaults(defineProps<Props>(), {
  height: 400,
  sortLabel: 'Sort by frequency',
  enableSort: true,
})

const chartContainer = ref<HTMLElement | null>(null)
const sortByFrequency = ref(false)

// Create unique tooltip ID for this component instance
const tooltipId = `realloc-gap-tooltip-${Math.random().toString(36).substr(2, 9)}`

// Create tooltip once and reuse it
let tooltip: d3.Selection<HTMLDivElement, unknown, HTMLElement, any> | null = null
let currentBar: SVGRectElement | null = null

const getOrCreateTooltip = () => {
  if (!tooltip || tooltip.empty()) {
    d3.selectAll(`.${tooltipId}`).remove()

    tooltip = d3
      .select('body')
      .append('div')
      .attr('class', `realloc-gap-tooltip ${tooltipId}`)
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

  const sortedData = sortByFrequency.value
    ? [...props.data].sort((a, b) => b.count - a.count)
    : [...props.data].sort((a, b) => {
        if (a.delta !== undefined && b.delta !== undefined) {
          return a.delta - b.delta
        }
        return 0 // Keep original order for label-based data
      })

  const x = d3
    .scaleBand()
    .domain(sortedData.map((d) => (d.delta !== undefined ? d.delta.toString() : d.label || '')))
    .range([0, width - margin.left - margin.right])
    .paddingInner(0.31)
    .paddingOuter(1)

  const y = d3
    .scaleLinear()
    .domain([0, d3.max(sortedData, (d) => d.count) || 0])
    .range([height, 0])

  const tooltipElement = getOrCreateTooltip()

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

  // Add bars
  svg
    .selectAll('.bar')
    .data(sortedData, (d: any) => (d.delta !== undefined ? d.delta.toString() : d.label || ''))
    .enter()
    .append('rect')
    .attr('class', 'bar')
    .attr('x', (d) => {
      const key = d.delta !== undefined ? d.delta.toString() : d.label || ''
      return x(key) || 0
    })
    .attr('y', (d) => y(d.count))
    .attr('width', x.bandwidth())
    .attr('height', (d) => height - y(d.count))
    .attr('fill', 'steelblue')
    .on('mouseover', function (_event, d) {
      currentBar = this as SVGRectElement

      d3.select(this).attr('fill', 'orangered')
      tooltipElement.transition().duration(200).style('opacity', 0.9)

      tooltipElement.html(
        `<strong>Frequency:</strong> <span style="color: red; font-weight: bold;">${humanReadable(d.count, 1, '', true)}</span>`,
      )

      updateTooltipPosition()
    })
    .on('mouseout', function () {
      d3.select(this).attr('fill', 'steelblue')
      tooltipElement.transition().duration(500).style('opacity', 0)
      currentBar = null
    })

  // X axis
  const xAxisGroup = svg
    .append('g')
    .attr('class', 'x axis')
    .attr('transform', `translate(0,${height})`)
    .call(
      d3.axisBottom(x).tickFormat((d) => {
        const delta = parseFloat(d as string)
        if (delta === 0 || !isFinite(delta)) {
          return 'others'
        }
        if (delta >= 0) {
          return humanReadable(delta, 1, 'B', true)
        }
        return '-' + humanReadable(-delta, 1, 'B', true)
      }),
    )

  xAxisGroup
    .selectAll('text')
    .style('text-anchor', 'end')
    .style('fill', 'black')
    .style('text-shadow', 'none')
    .style('font-weight', 'normal')
    .attr('dx', '-.8em')
    .attr('dy', '.15em')
    .attr('transform', 'rotate(-45)')

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
  yAxis
    .append('text')
    .attr('transform', 'rotate(-90)')
    .attr('y', 6)
    .attr('dy', '.71em')
    .style('text-anchor', 'end')
    .style('fill', 'black')
    .text('Frequency')
}

// Handle sort change with animation
const handleSortChange = (newSortValue: boolean) => {
  if (!chartContainer.value || props.data.length === 0) return

  const svg = d3.select(chartContainer.value).select('svg g')

  const sortedData = newSortValue
    ? [...props.data].sort((a, b) => b.count - a.count)
    : [...props.data].sort((a, b) => {
        if (a.delta !== undefined && b.delta !== undefined) {
          return a.delta - b.delta
        }
        return 0
      })

  const margin = { top: 20, right: 30, bottom: 60, left: 80 }
  const width = chartContainer.value.clientWidth || 800

  const x = d3
    .scaleBand()
    .domain(sortedData.map((d) => (d.delta !== undefined ? d.delta.toString() : d.label || '')))
    .range([0, width - margin.left - margin.right])
    .paddingInner(0.31)
    .paddingOuter(1)

  const transition = svg.transition().duration(750)
  const delay = (_: unknown, i: number) => i * 50

  transition
    .selectAll('.bar')
    .delay(delay)
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    .attr('x', (d: any) => {
      const key = d.delta !== undefined ? d.delta.toString() : d.label || ''
      return x(key) || 0
    })

  transition
    .select('.x.axis')
    .call(
      d3.axisBottom(x).tickFormat((d) => {
        const delta = parseFloat(d as string)
        if (delta === 0 || !isFinite(delta)) {
          return 'others'
        }
        if (delta >= 0) {
          return humanReadable(delta, 1, 'B', true)
        }
        return '-' + humanReadable(-delta, 1, 'B', true)
      }) as any,
    )
    .selectAll('g')
    .delay(delay)
}

onMounted(() => {
  buildChart()
  // Auto-check sort by frequency after 500ms (only if sorting is enabled)
  if (props.enableSort) {
    setTimeout(() => {
      sortByFrequency.value = true
    }, 500)
  }

  // Add scroll listener to update tooltip position
  window.addEventListener('scroll', handleScroll, true)
})

onBeforeUnmount(() => {
  d3.selectAll(`.${tooltipId}`).remove()
  window.removeEventListener('scroll', handleScroll, true)
})

watch(() => props.data, buildChart, { deep: true })
watch(sortByFrequency, (newValue, oldValue) => {
  if (oldValue !== undefined) {
    // Animate on user interaction (not on initial mount)
    handleSortChange(newValue)
  }
})
</script>

<style scoped>
.realloc-gap-chart-wrapper {
  width: 100%;
  overflow: auto;
}

.chart-controls {
  margin-bottom: 1rem;
}

.checkbox-label {
  display: flex;
  align-items: center;
  gap: 0.5rem;
  cursor: pointer;
  font-size: 0.875rem;
  color: #4b5563;
  user-select: none;
}

.checkbox-label input[type='checkbox'] {
  cursor: pointer;
  width: 16px;
  height: 16px;
}

.realloc-gap-chart {
  position: relative;
  width: 100%;
  min-height: 400px;
}
</style>

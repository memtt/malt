<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/charts/MostUsedSizesChart.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->

<template>
  <div class="most-used-sizes-chart-wrapper">
    <!-- Controls -->
    <div class="chart-controls">
      <label class="checkbox-label">
        <input type="checkbox" v-model="sortByFrequency" />
        <span>Sort values</span>
      </label>
    </div>

    <!-- Chart Container -->
    <div ref="chartContainer" class="most-used-sizes-chart"></div>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted, onBeforeUnmount, watch } from 'vue'
import * as d3 from 'd3'
import { humanReadable } from '@/lib/helpers'
import type { MostUsedSizeItem } from '@/types/alloc-size-distr'

interface Props {
  data: MostUsedSizeItem[]
  height?: number
}

const props = withDefaults(defineProps<Props>(), {
  height: 400,
})

const chartContainer = ref<HTMLElement | null>(null)
const sortByFrequency = ref(false)

// Create unique tooltip ID for this component instance
const tooltipId = `most-used-sizes-tooltip-${Math.random().toString(36).substr(2, 9)}`

// Create tooltip once and reuse it
let tooltip: d3.Selection<HTMLDivElement, unknown, HTMLElement, any> | null = null

const getOrCreateTooltip = () => {
  if (!tooltip || tooltip.empty()) {
    d3.selectAll(`.${tooltipId}`).remove()

    tooltip = d3
      .select('body')
      .append('div')
      .attr('class', `most-used-sizes-tooltip ${tooltipId}`)
      .style('position', 'fixed')
      .style('background', 'white')
      .style('padding', '8px 12px')
      .style('border', '1px solid #ddd')
      .style('border-radius', '4px')
      .style('pointer-events', 'none')
      .style('opacity', 0)
      .style('box-shadow', '0 2px 4px rgba(0,0,0,0.1)')
      .style('z-index', '9999')
      .style('font-size', '13px')
  }
  return tooltip
}

const updateTooltipPosition = (event: MouseEvent) => {
  if (!tooltip) return

  const tooltipNode = tooltip.node()
  if (tooltipNode) {
    const tooltipHeight = tooltipNode.getBoundingClientRect().height
    tooltip
      .style('left', `${event.clientX + 10}px`)
      .style('top', `${event.clientY - tooltipHeight / 2}px`)
  }
}

const buildChart = () => {
  if (!chartContainer.value || props.data.length === 0) return

  d3.select(chartContainer.value).selectAll('*').remove()

  const margin = { top: 20, right: 20, bottom: 30, left: 60 }
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

  // Scales
  const x = d3
    .scaleBand()
    .domain(props.data.map((d) => String(d.size)))
    .range([0, width - margin.left - margin.right])
    .padding(0.31)
    .paddingOuter(1)

  const y = d3
    .scaleLinear()
    .domain([0, d3.max(props.data, (d) => d.frequency) || 0])
    .range([height, 0])

  // Grid lines
  const gridLines = d3
    .axisLeft(y)
    .tickSize(-(width - margin.left - margin.right))
    .tickFormat(() => '')

  svg
    .append('g')
    .attr('class', 'grid')
    .call(gridLines)
    .selectAll('line')
    .style('stroke', '#e5e7eb')
    .style('stroke-opacity', 0.7)

  svg.selectAll('.grid .domain').remove()

  // X Axis
  const xAxis = svg
    .append('g')
    .attr('class', 'x axis')
    .attr('transform', `translate(0,${height})`)
    .call(
      d3.axisBottom(x).tickFormat((d) => {
        const size = Number(d)
        return size === 0 ? 'others' : humanReadable(size, 1, '', true)
      }),
    )

  xAxis
    .selectAll('text')
    .style('fill', 'black')
    .style('text-shadow', 'none')
    .style('font-weight', 'normal')

  xAxis.select('.domain').style('stroke', 'black')
  xAxis.selectAll('.tick line').style('stroke', 'black')

  // Y Axis
  const yAxis = svg
    .append('g')
    .attr('class', 'y axis')
    .call(d3.axisLeft(y).tickFormat((d) => humanReadable(Number(d), 1, '', false)))

  yAxis
    .selectAll('text')
    .style('fill', 'black')
    .style('text-shadow', 'none')
    .style('font-weight', 'normal')

  yAxis.select('.domain').style('stroke', 'black')
  yAxis.selectAll('.tick line').style('stroke', 'black')

  // Y Axis label
  svg
    .append('text')
    .attr('transform', 'rotate(-90)')
    .attr('x', -height / 2)
    .attr('y', -45)
    .attr('text-anchor', 'middle')
    .style('font-size', '12px')
    .style('fill', 'black')
    .text('Frequency')

  // Bars
  svg
    .selectAll('.bar')
    .data(props.data)
    .enter()
    .append('rect')
    .attr('class', 'bar')
    .attr('x', (d) => x(String(d.size)) || 0)
    .attr('width', x.bandwidth())
    .attr('y', (d) => y(d.frequency))
    .attr('height', (d) => height - y(d.frequency))
    .attr('fill', 'steelblue')
    .on('mouseover', function (event: MouseEvent, d) {
      d3.select(this).attr('fill', 'orangered')
      tooltipElement.transition().duration(200).style('opacity', 0.9)

      tooltipElement.html(
        `<strong>Frequency:</strong> <strong style='color:red'>${humanReadable(d.frequency, 1, '', true)}</strong>`,
      )

      updateTooltipPosition(event)
    })
    .on('mousemove', function (event: MouseEvent) {
      updateTooltipPosition(event)
    })
    .on('mouseout', function () {
      d3.select(this).attr('fill', 'steelblue')
      tooltipElement.transition().duration(500).style('opacity', 0)
    })

  // Sort functionality
  const sortTimeout = setTimeout(() => {
    sortByFrequency.value = true
  }, 500)

  watch(
    sortByFrequency,
    () => {
      clearTimeout(sortTimeout)

      const x0 = x
        .domain(
          props.data
            .slice()
            .sort((a, b) => {
              if (sortByFrequency.value) {
                return b.frequency - a.frequency
              } else {
                return a.size - b.size
              }
            })
            .map((d) => String(d.size)),
        )
        .copy()

      const transition = svg.transition().duration(750)
      const delay = (_d: any, i: number) => i * 50

      transition
        .selectAll('.bar')
        .delay(delay)
        .attr('x', (d: any) => x0(String(d.size)) || 0)

      transition.select('.x.axis').call(xAxis as any)
    },
    { immediate: false },
  )
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
.most-used-sizes-chart-wrapper {
  width: 100%;
  overflow: auto;
}

.chart-controls {
  margin-bottom: 1rem;
  padding: 0.5rem 0;
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

.most-used-sizes-chart {
  position: relative;
  width: 100%;
  min-height: 400px;
}

:deep(.grid line) {
  stroke: #e5e7eb;
  stroke-opacity: 0.7;
}

:deep(.bar) {
  transition: fill 0.2s;
}
</style>

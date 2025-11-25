<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/charts/HorizontalBarChart.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->

<template>
  <div class="bar-chart-wrapper">
    <!-- Legend and Controls -->
    <div class="chart-controls">
      <div class="series-legend">
        <div v-for="(series, index) in props.data" :key="series.key" class="legend-item">
          <div class="legend-color" :style="{ backgroundColor: series.color }"></div>
          <span class="legend-label">{{ series.key }}</span>
        </div>
      </div>
      <div class="display-mode">
        <label class="radio-label">
          <input type="radio" :checked="isStacked" @change="setStacked(true)" />
          <span>Stacked</span>
        </label>
        <label class="radio-label">
          <input type="radio" :checked="!isStacked" @change="setStacked(false)" />
          <span>Grouped</span>
        </label>
      </div>
    </div>

    <!-- Chart Container -->
    <div ref="chartContainer" class="bar-chart"></div>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted, onBeforeUnmount, watch, computed } from 'vue'
import * as d3 from 'd3'
import { humanReadable } from '@/lib/helpers'
import type { BarSeries, BarValue } from '@/types/global-vars'

interface Props {
  data: BarSeries[]
  height?: number
  stacked?: boolean
}

const props = withDefaults(defineProps<Props>(), {
  height: 400,
  stacked: true,
})

const emit = defineEmits<{
  valueClick: [event: MouseEvent, name: string]
}>()

const chartContainer = ref<HTMLElement | null>(null)
const isStacked = ref(props.stacked)

// Create unique tooltip ID for this component instance
const tooltipId = `bar-chart-tooltip-${Math.random().toString(36).substr(2, 9)}`

// Create tooltip once and reuse it
let tooltip: d3.Selection<HTMLDivElement, unknown, HTMLElement, any> | null = null

const dynamicHeight = computed(() => {
  if (!props.data || props.data.length === 0) return props.height
  const numBars = props.data[0]?.values?.length || 0
  return Math.max(props.height, 150 + numBars * 20)
})

const setStacked = (value: boolean) => {
  isStacked.value = value
  buildBarChart()
}

const formatBytes = (bytes: number): string => `${bytes.toLocaleString('en-US')} B`

const getOrCreateTooltip = () => {
  if (!tooltip || tooltip.empty()) {
    // Remove only this instance's tooltip
    d3.selectAll(`.${tooltipId}`).remove()

    tooltip = d3
      .select('body')
      .append('div')
      .attr('class', `bar-chart-tooltip ${tooltipId}`)
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

const getTooltipContent = (item: BarValue, series: BarSeries): string => {
  const position = item.file ? `<br/>${item.file}:${item.line}` : ''

  let tls = ''
  if (series.tlsInstances) {
    const perInstance = item.value / series.tlsInstances
    tls = `<br/>Instances: ${series.tlsInstances} × ${formatBytes(perInstance)}`
  }

  let used = ''
  if (item.usedSize !== undefined && !series.tlsInstances) {
    const percent = item.value > 0 ? ((item.usedSize / item.value) * 100).toFixed(1) : '0'
    used = `<br/>Accessed memory: ${formatBytes(item.usedSize)} (${percent}%)`
  }

  return `<div style="text-align:center"><strong style="margin:0 0 5px 0">${item.name}</strong><br />${formatBytes(item.value)}${tls}${used}${position}</div>`
}

const buildBarChart = () => {
  if (!chartContainer.value || props.data.length === 0) return

  d3.select(chartContainer.value).selectAll('*').remove()

  const margin = { top: 30, right: 120, bottom: 50, left: 80 }
  const width = chartContainer.value.clientWidth || 800
  const height = dynamicHeight.value - margin.top - margin.bottom

  const svg = d3
    .select(chartContainer.value)
    .append('svg')
    .attr('width', width)
    .attr('height', dynamicHeight.value)
    .append('g')
    .attr('transform', `translate(${margin.left},${margin.top})`)

  const barNames = props.data[0].values.map((d) => d.name)
  const y = d3.scaleBand().domain(barNames).range([0, height]).padding(0.2)

  const maxValue = isStacked.value
    ? (d3.max(barNames, (name) =>
        d3.sum(props.data, (series) => series.values.find((v) => v.name === name)?.value ?? 0),
      ) ?? 0)
    : (d3.max(props.data, (series) => d3.max(series.values, (d) => d.value) ?? 0) ?? 0)

  const x = d3
    .scaleLinear()
    .domain([0, maxValue])
    .range([0, width - margin.left - margin.right])

  const colorMap = Object.fromEntries(props.data.map((series) => [series.key, series.color]))

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

  svg
    .append('g')
    .attr('class', 'grid')
    .attr('transform', `translate(0,${height})`)
    .call(
      d3
        .axisBottom(x)
        .tickSize(-height)
        .tickFormat(() => ''),
    )
    .selectAll('line')
    .style('stroke', '#e5e7eb')
    .style('stroke-opacity', 0.7)

  svg.selectAll('.grid .domain').remove()

  const handleMouseOver = (
    element: SVGRectElement,
    event: MouseEvent,
    item: BarValue,
    series: BarSeries,
  ) => {
    d3.select(element).transition().duration(200).attr('opacity', 0.8)
    tooltipElement.transition().duration(200).style('opacity', 0.9)
    tooltipElement
      .html(getTooltipContent(item, series))
      .style('left', `${event.clientX + 10}px`)
      .style('top', `${event.clientY - 28}px`)
  }

  const handleMouseMove = (event: MouseEvent) => {
    tooltipElement.style('left', `${event.clientX + 10}px`).style('top', `${event.clientY - 28}px`)
  }

  const handleMouseOut = (element: SVGRectElement) => {
    d3.select(element).transition().duration(200).attr('opacity', 1)
    tooltipElement.transition().duration(500).style('opacity', 0)
  }

  if (isStacked.value) {
    barNames.forEach((name) => {
      let xOffset = 0

      props.data.forEach((series) => {
        const item = series.values.find((v) => v.name === name)
        if (!item || item.value === 0) return

        const barWidth = x(item.value)

        svg
          .append('rect')
          .attr('y', y(name)!)
          .attr('x', xOffset)
          .attr('height', y.bandwidth())
          .attr('width', barWidth)
          .attr('fill', colorMap[series.key])
          .style('cursor', 'pointer')
          .on('mouseover', function (event: MouseEvent) {
            handleMouseOver(this, event, item, series)
          })
          .on('mousemove', handleMouseMove)
          .on('mouseout', function () {
            handleMouseOut(this)
          })
          .on('click', (event: MouseEvent) => emit('valueClick', event, item.name))
          .transition()
          .duration(750)
          .attr('width', barWidth)

        if (barWidth > 40) {
          svg
            .append('text')
            .attr('y', y(name)! + y.bandwidth() / 2)
            .attr('x', xOffset + barWidth / 2)
            .attr('dy', '0.35em')
            .attr('text-anchor', 'middle')
            .style('fill', 'white')
            .style('font-size', '11px')
            .style('font-weight', 'bold')
            .style('pointer-events', 'none')
            .text(humanReadable(item.value, 1, 'B', true))
        }

        xOffset += barWidth
      })
    })
  } else {
    const seriesHeight = y.bandwidth() / props.data.length

    props.data.forEach((series, seriesIndex) => {
      series.values.forEach((item) => {
        const barWidth = x(item.value)
        const yPos = y(item.name)! + seriesIndex * seriesHeight

        svg
          .append('rect')
          .attr('y', yPos)
          .attr('x', 0)
          .attr('height', seriesHeight)
          .attr('width', 0)
          .attr('fill', colorMap[series.key])
          .style('cursor', 'pointer')
          .on('mouseover', function (event: MouseEvent) {
            handleMouseOver(this, event, item, series)
          })
          .on('mousemove', handleMouseMove)
          .on('mouseout', function () {
            handleMouseOut(this)
          })
          .on('click', (event: MouseEvent) => emit('valueClick', event, item.name))
          .transition()
          .duration(750)
          .attr('width', barWidth)

        if (barWidth > 5) {
          svg
            .append('text')
            .attr('y', yPos + seriesHeight / 2)
            .attr('x', barWidth + 5)
            .attr('dy', '0.35em')
            .attr('text-anchor', 'start')
            .style('fill', '#374151')
            .style('font-size', '11px')
            .style('font-weight', 'normal')
            .style('pointer-events', 'none')
            .text(formatBytes(item.value))
        }
      })
    })
  }

  const yAxis = svg.append('g').call(d3.axisLeft(y))
  yAxis
    .selectAll('text')
    .style('fill', 'black')
    .style('text-shadow', 'none')
    .style('font-weight', 'normal')
  yAxis.selectAll('line').style('stroke', 'black')
  yAxis.select('.domain').style('stroke', 'black')

  const xAxis = svg
    .append('g')
    .attr('transform', `translate(0,${height})`)
    .call(d3.axisBottom(x).tickFormat((d) => humanReadable(d as number, 1, 'B', true)))
  xAxis
    .selectAll('text')
    .style('fill', 'black')
    .style('text-shadow', 'none')
    .style('font-weight', 'normal')
  xAxis.selectAll('line').style('stroke', 'black')
  xAxis.select('.domain').style('stroke', 'black')
}

onMounted(buildBarChart)

onBeforeUnmount(() => {
  // Remove only this instance's tooltip
  d3.selectAll(`.${tooltipId}`).remove()
})

watch(() => props.data, buildBarChart, { deep: true })
watch(dynamicHeight, buildBarChart)
</script>

<style scoped>
.bar-chart-wrapper {
  width: 100%;
  overflow: auto;
}

.chart-controls {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 12px 16px;
  background-color: #f9fafb;
  border: 1px solid #e5e7eb;
  border-radius: 6px 6px 0 0;
  margin-bottom: -1px;
}

.series-legend {
  display: flex;
  gap: 24px;
  align-items: center;
}

.series-legend .legend-item {
  display: flex;
  align-items: center;
  gap: 8px;
}

.series-legend .legend-color {
  width: 16px;
  height: 16px;
  border-radius: 3px;
}

.series-legend .legend-label {
  font-size: 14px;
  font-weight: normal;
  color: #374151;
}

.display-mode {
  display: flex;
  gap: 16px;
  align-items: center;
}

.radio-label {
  display: flex;
  align-items: center;
  gap: 6px;
  cursor: pointer;
  font-size: 14px;
  color: #4b5563;
  user-select: none;
}

.radio-label input[type='radio'] {
  cursor: pointer;
  width: 16px;
  height: 16px;
}

.radio-label:hover {
  color: #1f2937;
}

.bar-chart {
  position: relative;
  width: 100%;
  min-height: 300px;
  background: white;
  border: 1px solid #e5e7eb;
  border-radius: 0 0 6px 6px;
  padding: 8px;
}
</style>

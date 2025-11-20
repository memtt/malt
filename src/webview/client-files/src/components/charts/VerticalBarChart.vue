<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/charts/VerticalBarChart.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->
<template>
  <div class="vertical-bar-chart-wrapper">
    <!-- Chart Container -->
    <div ref="chartContainer" class="vertical-bar-chart"></div>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted, onBeforeUnmount, watch } from 'vue'
import * as d3 from 'd3'
import type { BarSeries, BarValue } from '@/types/global-vars'

interface Props {
  data: BarSeries[]
  height?: number
}

const props = withDefaults(defineProps<Props>(), {
  height: 500,
})

const chartContainer = ref<HTMLElement | null>(null)

const createTooltip = () =>
  d3
    .select('body')
    .append('div')
    .attr('class', 'vertical-bar-chart-tooltip')
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

const buildBarChart = () => {
  if (!chartContainer.value || props.data.length === 0) return

  d3.select(chartContainer.value).selectAll('*').remove()
  d3.selectAll('.vertical-bar-chart-tooltip').remove()

  const margin = { top: 20, right: 30, bottom: 100, left: 80 }
  const width = chartContainer.value.clientWidth || 800
  const height = props.height - margin.top - margin.bottom

  const svg = d3
    .select(chartContainer.value)
    .append('svg')
    .attr('width', width)
    .attr('height', props.height)
    .append('g')
    .attr('transform', `translate(${margin.left},${margin.top})`)

  const barNames = props.data[0].values.map((d) => d.name)

  const x = d3
    .scaleBand()
    .domain(barNames)
    .range([0, width - margin.left - margin.right])
    .padding(0.2)

  const maxValue = d3.max(props.data[0].values, (d) => d.value) || 0

  const y = d3.scaleLinear().domain([0, maxValue]).range([height, 0])

  const colorMap = Object.fromEntries(props.data.map((series) => [series.key, series.color]))

  const tooltip = createTooltip()

  const series = props.data[0]

  series.values.forEach((item) => {
    const barWidth = x.bandwidth()
    const xPos = x(item.name)!
    const barHeight = height - y(item.value)

    svg
      .append('rect')
      .attr('x', xPos)
      .attr('y', height)
      .attr('width', barWidth)
      .attr('height', 0)
      .attr('fill', colorMap[series.key])
      .style('cursor', 'pointer')
      .on('mouseover', function (event: MouseEvent) {
        d3.select(this).transition().duration(200).attr('opacity', 0.8)
        tooltip.transition().duration(200).style('opacity', 0.9)
        tooltip
          .html(
            `<div style="text-align:center"><h3 style="margin:0 0 5px 0">${item.name}</h3>${item.value.toLocaleString()}</div>`,
          )
          .style('left', `${event.clientX + 10}px`)
          .style('top', `${event.clientY - 28}px`)
      })
      .on('mousemove', function (event: MouseEvent) {
        tooltip.style('left', `${event.clientX + 10}px`).style('top', `${event.clientY - 28}px`)
      })
      .on('mouseout', function () {
        d3.select(this).transition().duration(200).attr('opacity', 1)
        tooltip.transition().duration(500).style('opacity', 0)
      })
      .transition()
      .duration(750)
      .attr('y', y(item.value))
      .attr('height', barHeight)

    // Add value labels on top of bars
    if (barHeight > 20) {
      svg
        .append('text')
        .attr('x', xPos + barWidth / 2)
        .attr('y', y(item.value) - 5)
        .attr('text-anchor', 'middle')
        .style('fill', '#374151')
        .style('font-size', '11px')
        .style('font-weight', '500')
        .style('pointer-events', 'none')
        .text(item.value.toLocaleString())
    }
  })

  // X axis
  svg
    .append('g')
    .attr('transform', `translate(0,${height})`)
    .call(d3.axisBottom(x))
    .selectAll('text')
    .style('text-anchor', 'end')
    .attr('dx', '-.8em')
    .attr('dy', '.15em')
    .attr('transform', 'rotate(-45)')

  // Y axis
  svg.append('g').call(d3.axisLeft(y))
}

onMounted(buildBarChart)

onBeforeUnmount(() => {
  d3.selectAll('.vertical-bar-chart-tooltip').remove()
})

watch(() => props.data, buildBarChart, { deep: true })
</script>

<style scoped>
.vertical-bar-chart-wrapper {
  width: 100%;
}

.vertical-bar-chart {
  position: relative;
  width: 100%;
  min-height: 300px;
  background: white;
  border: 1px solid #e5e7eb;
  border-radius: 6px;
  padding: 8px;
  overflow: auto;
}
</style>

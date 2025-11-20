<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/charts/DonutChart.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->

<template>
  <div class="donut-chart-wrapper">
    <div ref="chartContainer" class="donut-chart"></div>
    <div class="legend-container">
      <div
        v-for="(item, index) in sortedLegendData"
        :key="item.name"
        class="legend-item"
        @click="onLegendClick(item.name)"
        :style="{ cursor: 'pointer' }"
      >
        <div class="legend-color" :style="{ backgroundColor: getColor(index) }"></div>
        <div class="legend-text">
          <span class="legend-name">{{ item.name }}</span>
          <span class="legend-value">{{ humanReadable(item.value, 1, 'B', true) }}</span>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted, onBeforeUnmount, watch, computed } from 'vue'
import * as d3 from 'd3'
import { humanReadable } from '@/lib/helpers'
import type { PieData } from '@/types/global-vars'

interface Props {
  data: PieData[]
  height?: number
  clickable?: boolean
}

const props = withDefaults(defineProps<Props>(), {
  height: 400,
  clickable: false,
})

const emit = defineEmits<{
  legendClick: [name: string]
}>()

const chartContainer = ref<HTMLElement | null>(null)
const colorScale = ref<d3.ScaleOrdinal<string, string, never>>()

// Create unique tooltip ID for this component instance
const tooltipId = `donut-chart-tooltip-${Math.random().toString(36).substr(2, 9)}`

// Create tooltip once and reuse it
let tooltip: d3.Selection<HTMLDivElement, unknown, HTMLElement, any> | null = null

const sortedLegendData = computed(() => [...props.data].sort((a, b) => b.value - a.value))

const getColor = (index: number): string => colorScale.value?.(index.toString()) ?? '#ccc'

const onLegendClick = (name: string) => {
  if (props.clickable) {
    emit('legendClick', name)
  }
}

const createSvg = (width: number, height: number) => {
  return d3
    .select(chartContainer.value)
    .append('svg')
    .attr('width', width)
    .attr('height', height)
    .append('g')
    .attr('transform', `translate(${width / 2},${height / 2})`)
}

const getOrCreateTooltip = () => {
  if (!tooltip || tooltip.empty()) {
    // Remove only this instance's tooltip
    d3.selectAll(`.${tooltipId}`).remove()

    tooltip = d3
      .select('body')
      .append('div')
      .attr('class', `donut-chart-tooltip ${tooltipId}`)
      .style('position', 'fixed')
      .style('background', 'white')
      .style('padding', '12px')
      .style('border', '1px solid #ddd')
      .style('border-radius', '6px')
      .style('pointer-events', 'none')
      .style('opacity', 0)
      .style('box-shadow', '0 4px 6px rgba(0,0,0,0.1)')
      .style('z-index', '9999')
      .style('font-size', '13px')
  }
  return tooltip
}

const buildDonutChart = () => {
  if (!chartContainer.value || props.data.length === 0) return

  d3.select(chartContainer.value).selectAll('*').remove()

  const width = chartContainer.value.clientWidth || 400
  const height = props.height
  const radius = Math.min(width, height) / 2

  const svg = createSvg(width, height)
  const color = d3.scaleOrdinal(d3.schemeCategory10)
  colorScale.value = color

  const tooltipElement = getOrCreateTooltip()

  const pie = d3
    .pie<PieData>()
    .value((d) => d.value)
    .sort(null)

  const arc = d3
    .arc<d3.PieArcDatum<PieData>>()
    .innerRadius(radius * 0.65)
    .outerRadius(radius * 0.9)

  const labelArc = d3
    .arc<d3.PieArcDatum<PieData>>()
    .innerRadius(radius * 0.95)
    .outerRadius(radius * 0.95)

  const totalValue = d3.sum(props.data, (item) => item.value)!

  const slices = svg
    .selectAll('path')
    .data(pie(props.data))
    .enter()
    .append('path')
    .attr('d', arc)
    .attr('fill', (_, i) => color(i.toString()))
    .attr('stroke', 'white')
    .style('stroke-width', '2px')
    .style('cursor', 'pointer')
    .on('mouseover', function (event: MouseEvent, d: d3.PieArcDatum<PieData>) {
      d3.select(this).transition().duration(200).attr('opacity', 0.8)

      const position = d.data.file ? `<br/>${d.data.file}:${d.data.line}` : ''

      tooltipElement.transition().duration(200).style('opacity', 0.9)
      tooltipElement
        .html(
          `<strong style="margin:0 0 5px 0">${d.data.name}</strong><br />${humanReadable(d.data.value, 1, 'B', true)}${position}`,
        )
        .style('left', `${event.clientX + 10}px`)
        .style('top', `${event.clientY - 28}px`)
    })
    .on('mousemove', function (event: MouseEvent) {
      tooltipElement
        .style('left', `${event.clientX + 10}px`)
        .style('top', `${event.clientY - 28}px`)
    })
    .on('mouseout', function () {
      d3.select(this).transition().duration(200).attr('opacity', 1)
      tooltipElement.transition().duration(500).style('opacity', 0)
    })

  svg
    .selectAll('text')
    .data(pie(props.data))
    .enter()
    .append('text')
    .attr('transform', (d) => `translate(${labelArc.centroid(d)})`)
    .attr('dy', '0.35em')
    .attr('text-anchor', 'middle')
    .style('fill', '#333')
    .style('font-size', '12px')
    .style('font-weight', 'normal')
    .style('pointer-events', 'none')
    .text((d) => {
      const percent = ((d.data.value / totalValue) * 100).toFixed(1)
      return Number(percent) > 5 ? `${percent}%` : ''
    })

  slices
    .transition()
    .duration(750)
    .attrTween('d', function (d) {
      const interpolate = d3.interpolate({ startAngle: 0, endAngle: 0 }, d)
      return (t: number) => arc(interpolate(t) as any) || ''
    })
}

onMounted(buildDonutChart)

onBeforeUnmount(() => {
  // Remove only this instance's tooltip
  d3.selectAll(`.${tooltipId}`).remove()
})

watch(() => props.data, buildDonutChart, { deep: true })
</script>

<style scoped>
.donut-chart-wrapper {
  display: flex;
  gap: 2rem;
  align-items: flex-start;
}

.donut-chart {
  position: relative;
  flex: 1;
  min-height: 300px;
}

.legend-container {
  flex: 0 0 250px;
  max-height: 400px;
  overflow-y: auto;
  padding: 10px;
  border-left: 1px solid #e5e7eb;
}

.legend-item {
  display: flex;
  align-items: center;
  gap: 8px;
  padding: 6px 8px;
  margin-bottom: 4px;
  border-radius: 4px;
  transition: background-color 0.2s;
}

.legend-item:hover {
  background-color: #f3f4f6;
}

.legend-color {
  width: 16px;
  height: 16px;
  border-radius: 3px;
  flex-shrink: 0;
}

.legend-text {
  display: flex;
  flex-direction: column;
  gap: 2px;
  flex: 1;
  min-width: 0;
}

.legend-name {
  font-size: 13px;
  font-weight: 500;
  color: #374151;
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
}

.legend-value {
  font-size: 12px;
  color: #6b7280;
}
</style>

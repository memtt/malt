<template>
  <div class="realloc-bundle-chart-wrapper">
    <!-- Legend (positioned top-right) -->
    <div class="chart-legend">
      <div class="legend-title">Hover any size to highlight:</div>
      <div class="legend-item">
        <svg width="30" height="12">
          <line x1="0" y1="6" x2="30" y2="6" stroke="#d62728" stroke-width="2" />
        </svg>
        <span>Outgoing</span>
      </div>
      <div class="legend-item">
        <svg width="30" height="12">
          <line
            x1="0"
            y1="6"
            x2="30"
            y2="6"
            stroke="#2ca02c"
            stroke-width="2"
            stroke-dasharray="3,2"
          />
        </svg>
        <span>Incoming</span>
      </div>
    </div>
    <!-- Chart Container -->
    <div ref="chartContainer" class="realloc-bundle-chart"></div>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted, onBeforeUnmount, watch } from 'vue'
import * as d3 from 'd3'
import { humanReadable } from '@/lib/helpers'

interface ReallocData {
  oldSize: number
  newSize: number
  count: number
}

interface Props {
  data: ReallocData[]
  diameter?: number
}

const props = withDefaults(defineProps<Props>(), {
  diameter: 960,
})

const chartContainer = ref<HTMLElement | null>(null)

interface NodeData {
  size: number
  angle: number
  x: number
  y: number
  source?: boolean
  target?: boolean
}

const buildChart = () => {
  if (!chartContainer.value || props.data.length === 0) return

  d3.select(chartContainer.value).selectAll('*').remove()

  const diameter = props.diameter
  const radius = diameter / 2
  const outerRadius = radius - 120 // Where nodes are placed
  const innerRadius = radius - 300 // Where lines stop (creating empty center)

  // Build unique sizes and sort them
  const sizeSet = new Set<number>()
  props.data.forEach((d) => {
    sizeSet.add(d.oldSize)
    sizeSet.add(d.newSize)
  })

  const sizes = Array.from(sizeSet).sort((a, b) => a - b)
  const angleStep = 360 / sizes.length

  // Create node map with positions
  const nodeMap: Record<number, NodeData> = {}
  sizes.forEach((size, i) => {
    const angle = i * angleStep
    nodeMap[size] = {
      size,
      angle,
      x: angle,
      y: outerRadius,
    }
  })

  const svg = d3
    .select(chartContainer.value)
    .append('svg')
    .attr('width', diameter)
    .attr('height', diameter)

  // Define arrow markers
  const defs = svg.append('defs')

  // Red arrow for source links
  defs
    .append('marker')
    .attr('id', 'arrow-source')
    .attr('viewBox', '0 -5 10 10')
    .attr('refX', 8)
    .attr('refY', 0)
    .attr('markerWidth', 4)
    .attr('markerHeight', 4)
    .attr('orient', 'auto')
    .append('path')
    .attr('d', 'M0,-5L10,0L0,5')
    .attr('fill', '#d62728')

  // Green arrow for target links (dashed won't affect arrow)
  defs
    .append('marker')
    .attr('id', 'arrow-target')
    .attr('viewBox', '0 -5 10 10')
    .attr('refX', 8)
    .attr('refY', 0)
    .attr('markerWidth', 4)
    .attr('markerHeight', 4)
    .attr('orient', 'auto')
    .append('path')
    .attr('d', 'M0,-5L10,0L0,5')
    .attr('fill', '#2ca02c')

  const g = svg.append('g').attr('transform', `translate(${radius},${radius})`)

  // Create radial lines with empty center
  const linkGroup = g.append('g')

  props.data.forEach((d) => {
    const source = nodeMap[d.oldSize]
    const target = nodeMap[d.newSize]

    const sourceAngle = (source.angle - 90) * (Math.PI / 180)
    const targetAngle = (target.angle - 90) * (Math.PI / 180)

    // Outer points (where nodes are)
    const sourceX = Math.cos(sourceAngle) * outerRadius
    const sourceY = Math.sin(sourceAngle) * outerRadius
    const targetX = Math.cos(targetAngle) * outerRadius
    const targetY = Math.sin(targetAngle) * outerRadius

    // Inner points (where lines stop, creating empty center)
    const sourceInnerX = Math.cos(sourceAngle) * innerRadius
    const sourceInnerY = Math.sin(sourceAngle) * innerRadius
    const targetInnerX = Math.cos(targetAngle) * innerRadius
    const targetInnerY = Math.sin(targetAngle) * innerRadius

    // Create a curved path using bundle curve
    const lineGenerator = d3
      .line()
      .curve(d3.curveBundle.beta(0.85))
      .x((d: any) => d[0])
      .y((d: any) => d[1])

    const pathData = [
      [sourceX, sourceY],
      [sourceInnerX, sourceInnerY],
      [targetInnerX, targetInnerY],
      [targetX, targetY],
    ]

    const path = linkGroup
      .append('path')
      .attr('class', 'link')
      .attr('d', lineGenerator(pathData as any))
      .style('fill', 'none')
      .style('stroke', 'steelblue')
      .style('stroke-opacity', 0.4)
      .style('stroke-width', 1.5)

    ;(path.node() as any).__data__ = {
      oldSize: d.oldSize,
      newSize: d.newSize,
      count: d.count,
    }
  })

  const link = linkGroup.selectAll('.link')

  // Create nodes as text labels
  const node = g
    .append('g')
    .selectAll('.node')
    .data(sizes)
    .enter()
    .append('text')
    .attr('class', 'node')
    .attr('dy', '.31em')
    .attr('transform', (size) => {
      const nodeData = nodeMap[size]
      const angle = nodeData.angle - 90
      const rotate = nodeData.angle < 180 ? '' : 'rotate(180)'
      return `rotate(${angle})translate(${nodeData.y + 8},0)${rotate}`
    })
    .style('text-anchor', (size) => (nodeMap[size].angle < 180 ? 'start' : 'end'))
    .style('font-size', '11px')
    .style('fill', 'black')
    .text((size) => humanReadable(size, 1, 'B', true))
    .on('mouseover', function (_event, size) {
      mouseovered(size, node, link, nodeMap)
    })
    .on('mouseout', function () {
      mouseouted(node, link, nodeMap)
    })

  function mouseovered(
    hoveredSize: number,
    nodeSelection: any,
    linkSelection: any,
    nodeMap: Record<number, NodeData>,
  ) {
    // Reset all nodes
    Object.values(nodeMap).forEach((n) => {
      n.source = false
      n.target = false
    })

    // Highlight connected links
    linkSelection
      .classed('link--target', function (this: any) {
        const d = this.__data__
        if (d.newSize === hoveredSize) {
          nodeMap[d.oldSize].source = true
          d3.select(this).attr('marker-mid', 'url(#arrow-target)')
          return true
        }
        return false
      })
      .classed('link--source', function (this: any) {
        const d = this.__data__
        if (d.oldSize === hoveredSize) {
          nodeMap[d.newSize].target = true
          d3.select(this).attr('marker-mid', 'url(#arrow-source)')
          return true
        }
        return false
      })
      .filter(function (this: any) {
        const d = this.__data__
        return d.newSize === hoveredSize || d.oldSize === hoveredSize
      })
      .each(function (this: SVGPathElement) {
        // Bring to front
        this.parentNode?.appendChild(this)
      })

    // Style nodes
    nodeSelection
      .classed('node--target', (size: number) => nodeMap[size].target)
      .classed('node--source', (size: number) => nodeMap[size].source)
  }

  function mouseouted(nodeSelection: any, linkSelection: any, nodeMap: Record<number, NodeData>) {
    linkSelection
      .classed('link--target', false)
      .classed('link--source', false)
      .attr('marker-mid', null)
    nodeSelection.classed('node--target', false).classed('node--source', false)

    // Reset node flags
    Object.values(nodeMap).forEach((n) => {
      n.source = false
      n.target = false
    })
  }
}

onMounted(() => {
  buildChart()
})

onBeforeUnmount(() => {
  // Cleanup
})

watch(() => props.data, buildChart, { deep: true })
</script>

<style scoped>
.realloc-bundle-chart-wrapper {
  position: relative;
  width: 100%;
  overflow: auto;
}

.realloc-bundle-chart {
  position: relative;
  width: 100%;
  min-height: 600px;
  display: flex;
  justify-content: center;
  align-items: center;
}

.chart-legend {
  position: absolute;
  top: 20px;
  right: 20px;
  display: flex;
  flex-direction: column;
  gap: 0.5rem;
  padding: 0.75rem 1rem;
  background: rgba(255, 255, 255, 0.95);
  border: 1px solid #d1d5db;
  border-radius: 6px;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
  z-index: 10;
}

.legend-title {
  font-size: 0.75rem;
  font-weight: 600;
  color: #374151;
  margin-bottom: 0.25rem;
}

.legend-item {
  display: flex;
  align-items: center;
  gap: 0.5rem;
}

.legend-item span {
  font-size: 0.75rem;
  color: #6b7280;
}

/* Link styling */
:deep(.link) {
  stroke: steelblue;
  stroke-opacity: 0.4;
  stroke-width: 1.5px;
}

:deep(.link--source) {
  stroke: #d62728 !important;
  stroke-opacity: 1 !important;
  stroke-width: 2px !important;
}

:deep(.link--target) {
  stroke: #2ca02c !important;
  stroke-opacity: 1 !important;
  stroke-width: 2px !important;
  stroke-dasharray: 5, 5 !important;
}

/* Node styling */
:deep(.node) {
  font-size: 11px;
  fill: #000;
}

:deep(.node--source) {
  fill: #d62728 !important;
  font-weight: bold;
}

:deep(.node--target) {
  fill: #2ca02c !important;
  font-weight: bold;
}
</style>

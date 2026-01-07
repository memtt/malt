<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/charts/GlobalVarsTreemapChart.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->

<template>
  <div ref="containerRef" class="treemap-container"></div>
</template>

<script setup lang="ts">
import { ref, onMounted, watch, onBeforeUnmount } from 'vue'
import * as d3 from 'd3'
import { humanReadable } from '@/lib/helpers'

interface TreemapNode {
  name: string
  value?: number
  children?: TreemapNode[]
}

interface Props {
  data: TreemapNode
  height?: number
}

const props = withDefaults(defineProps<Props>(), {
  height: 600,
})

const containerRef = ref<HTMLDivElement | null>(null)
let svg: d3.Selection<SVGSVGElement, unknown, null, undefined> | null = null
const tooltipId = `treemap-tooltip-${Math.random().toString(36).substr(2, 9)}`

const drawTreemap = () => {
  if (!containerRef.value || !props.data) return

  // Clear previous chart
  d3.select(containerRef.value).selectAll('*').remove()

  // Remove existing tooltip
  d3.select(`#${tooltipId}`).remove()

  const container = containerRef.value
  const width = container.clientWidth
  const height = props.height

  // Color scale based on top-level children
  const color = d3.scaleOrdinal(props.data.children?.map((d) => d.name) || [], d3.schemeTableau10)

  // Compute the layout
  const root = d3
    .treemap<TreemapNode>()
    .tile(d3.treemapSquarify)
    .size([width, height])
    .padding(1)
    .round(true)(
    d3
      .hierarchy(props.data)
      .sum((d) => d.value || 0)
      .sort((a, b) => (b.value || 0) - (a.value || 0)),
  )

  // Create SVG
  svg = d3
    .select(container)
    .append('svg')
    .attr('viewBox', [0, 0, width, height])
    .attr('width', width)
    .attr('height', height)
    .attr('style', 'max-width: 100%; height: auto; font: 10px sans-serif;')

  // Create tooltip
  const tooltip = d3
    .select('body')
    .append('div')
    .attr('id', tooltipId)
    .attr('class', 'treemap-tooltip')
    .style('position', 'fixed')
    .style('visibility', 'hidden')
    .style('background', 'var(--color-primary, #184862)')
    .style('color', 'white')
    .style('padding', '8px 12px')
    .style('border-radius', 'var(--radius-md, 8px)')
    .style('font-size', 'var(--font-size-sm, 14px)')
    .style('font-family', 'var(--font-family-base, Maven Pro, sans-serif)')
    .style('pointer-events', 'none')
    .style('z-index', '9999')
    .style('box-shadow', '0 2px 8px rgba(0, 0, 0, 0.15)')

  // Add cells for each leaf
  const leaf = svg
    .selectAll('g')
    .data(root.leaves())
    .join('g')
    .attr('transform', (d) => `translate(${d.x0},${d.y0})`)
    .style('cursor', 'pointer')

  // Helper functions for tooltip interactions
  const showTooltip = function (this: any, event: MouseEvent, d: any) {
    // Highlight the corresponding rectangle
    const parent = d3.select(this.parentNode || this)
    parent.select('rect').attr('fill-opacity', 0.8)

    // Remove index suffix (#123) from names in path
    const path = d
      .ancestors()
      .reverse()
      .map((d: any) => d.data.name.replace(/#\d+$/, ''))
      .join(' > ')
    tooltip
      .html(`${path}<br/><strong>${humanReadable(d.value || 0, 1, 'B', true)}</strong>`)
      .style('visibility', 'visible')
      .style('left', `${event.clientX + 10}px`)
      .style('top', `${event.clientY + 10}px`)
  }

  const moveTooltip = function (event: MouseEvent) {
    tooltip.style('left', `${event.clientX + 10}px`).style('top', `${event.clientY + 10}px`)
  }

  const hideTooltip = function (this: any) {
    // Remove highlight from the corresponding rectangle
    const parent = d3.select(this.parentNode || this)
    parent.select('rect').attr('fill-opacity', 0.6)
    tooltip.style('visibility', 'hidden')
  }

  // Add rectangles
  leaf
    .append('rect')
    .attr('fill', (d) => {
      let node = d
      while (node.depth > 1) node = node.parent!
      return color(node.data.name)
    })
    .attr('fill-opacity', 0.6)
    .attr('width', (d) => d.x1 - d.x0)
    .attr('height', (d) => d.y1 - d.y0)
    .attr('stroke', 'white')
    .attr('stroke-width', 1)
    .on('mouseenter', showTooltip)
    .on('mousemove', moveTooltip)
    .on('mouseleave', hideTooltip)

  // Add clipPath for text
  leaf
    .append('clipPath')
    .attr('id', (d, i) => `clip-${tooltipId}-${i}`)
    .append('rect')
    .attr('width', (d) => d.x1 - d.x0)
    .attr('height', (d) => d.y1 - d.y0)

  // Add text
  leaf
    .append('text')
    .attr('clip-path', (d, i) => `url(#clip-${tooltipId}-${i})`)
    .style('pointer-events', 'all')
    .style('cursor', 'pointer')
    .on('mouseenter', showTooltip)
    .on('mousemove', moveTooltip)
    .on('mouseleave', hideTooltip)
    .selectAll('tspan')
    .data((d) => {
      // Remove index suffix (#123) from name
      const name = d.data.name.replace(/#\d+$/, '')
      const parts = name.split(/(?=[A-Z][a-z])|\s+/g)
      return [...parts, humanReadable(d.value || 0, 1, 'B', true)]
    })
    .join('tspan')
    .attr('x', 3)
    .attr('y', (d, i, nodes) => `${(i === nodes.length - 1 ? 0.3 : 0) + 1.1 + i * 0.9}em`)
    .attr('fill-opacity', (d, i, nodes) => (i === nodes.length - 1 ? 0.7 : 1))
    .attr('font-weight', (d, i, nodes) => (i === nodes.length - 1 ? 'bold' : 'normal'))
    .text((d) => d)
}

onMounted(() => {
  drawTreemap()
  window.addEventListener('resize', drawTreemap)
})

onBeforeUnmount(() => {
  window.removeEventListener('resize', drawTreemap)
  d3.select(`#${tooltipId}`).remove()
})

watch(() => [props.data, props.height], drawTreemap, { deep: true })
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.treemap-container {
  width: 100%;
  min-height: 400px;
  overflow: auto;

  // Define CSS custom properties for d3 inline styles
  --color-primary: #{$color-primary};
  --radius-md: #{$radius-md};
  --font-size-sm: #{$font-size-sm};
  --font-family-base: #{$font-family-base};
}
</style>

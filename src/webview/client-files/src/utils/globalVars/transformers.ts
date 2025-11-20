/**
 * Global Variables Transformers
 * Pure functions for data transformations
 */

import type { GlobalVarsData, PieData } from '@/types/global-vars'

export interface TreemapNode {
  name: string
  value?: number
  children?: TreemapNode[]
}

/**
 * Transform data for Binary Treemap
 * Shows all binaries at the top level with their total size
 */
export function transformToBinaryTreemap(
  data: GlobalVarsData | null,
  tlsInstances: number,
): TreemapNode {
  if (!data) {
    return { name: 'root', children: [] }
  }

  const children: TreemapNode[] = []

  for (const binaryFile in data.vars) {
    const binaryName = binaryFile.split('/').pop() || binaryFile

    // Calculate total size for this binary
    let totalSize = 0
    for (const variable of data.vars[binaryFile]) {
      const size = variable.tls ? variable.size * tlsInstances : variable.size
      totalSize += size
    }

    // Add binary as a leaf node with total size
    if (totalSize > 0) {
      children.push({
        name: binaryName,
        value: totalSize,
      })
    }
  }

  return {
    name: 'root',
    children,
  }
}

/**
 * Transform data for Variable Treemap
 * Shows ALL variables at the top level (no deduplication, all instances shown)
 * Variables < 0.5% are grouped into "Autres"
 */
export function transformToVariableTreemap(
  data: GlobalVarsData | null,
  tlsInstances: number,
): TreemapNode {
  if (!data) {
    return { name: 'root', children: [] }
  }

  // First, calculate total size to determine percentages
  let totalSize = 0
  for (const binaryFile in data.vars) {
    for (const variable of data.vars[binaryFile]) {
      const size = variable.tls ? variable.size * tlsInstances : variable.size
      totalSize += size
    }
  }

  const children: TreemapNode[] = []
  let index = 0
  let autresSize = 0
  const threshold = totalSize * 0.001 // 0.5%

  // Add ALL variables as direct children (no grouping, no deduplication)
  // Group variables < 0.5% into "Autres"
  for (const binaryFile in data.vars) {
    for (const variable of data.vars[binaryFile]) {
      const size = variable.tls ? variable.size * tlsInstances : variable.size

      if (size < threshold) {
        autresSize += size
      } else {
        children.push({
          name: `${variable.name}#${index++}`, // Make unique with index
          value: size,
        })
      }
    }
  }

  // Add "Autres" category if there are small variables
  if (autresSize > 0) {
    children.push({
      name: 'Autres',
      value: autresSize,
    })
  }

  return {
    name: 'root',
    children,
  }
}

/**
 * Transform data for Binary Pie Chart
 */
export function transformToBinaryPieData(
  data: GlobalVarsData | null,
  tlsInstances: number,
): PieData[] {
  if (!data) return []

  const res: PieData[] = []

  for (const bin in data.vars) {
    let cnt = 0
    for (const v of data.vars[bin]) {
      if (v.tls) {
        cnt += v.size * tlsInstances
      } else {
        cnt += v.size
      }
    }
    res.push({ name: bin.split('/').pop() || bin, value: cnt })
  }

  return res
}

/**
 * Transform data for Variable Pie Chart
 */
export function transformToVarPieData(
  data: GlobalVarsData | null,
  tlsInstances: number,
): PieData[] {
  if (!data) return []

  const res: PieData[] = []

  for (const bin in data.vars) {
    for (const v of data.vars[bin]) {
      let size = v.size
      if (v.tls) {
        size *= tlsInstances
      }
      res.push({
        name: v.name,
        value: size,
        file: v.sourceFile,
        line: v.line,
        usedSize: v.usedSize,
      })
    }
  }

  return res
}

export interface BarChartRawData {
  name: string
  tls: number
  gbl: number
  file?: string
  line?: number
  usedSize: number
}

/**
 * Transform data for Binary Bar Chart
 */
export function transformToBinaryBarData(
  data: GlobalVarsData | null,
  tlsInstances: number,
): BarChartRawData[] {
  if (!data) return []

  const res: BarChartRawData[] = []

  for (const bin in data.vars) {
    let cntTls = 0
    let cntGbl = 0
    let usedSize = 0

    for (const v of data.vars[bin]) {
      if (v.tls) {
        cntTls += v.size * tlsInstances
      } else {
        cntGbl += v.size
      }
      usedSize += v.usedSize || 0
    }

    res.push({
      name: bin.split('/').pop() || bin,
      tls: cntTls,
      gbl: cntGbl,
      usedSize: usedSize,
    })
  }

  return res
}

/**
 * Transform data for Variable Bar Chart
 */
export function transformToVarBarData(
  data: GlobalVarsData | null,
  tlsInstances: number,
  filter?: (name: string) => boolean,
): BarChartRawData[] {
  if (!data) return []

  const res: BarChartRawData[] = []

  for (const bin in data.vars) {
    const fname = bin.split('/').pop() || bin
    if (!filter || filter(fname)) {
      for (const v of data.vars[bin]) {
        if (v.tls) {
          res.push({
            name: v.name,
            tls: v.size * tlsInstances,
            gbl: 0,
            file: v.sourceFile,
            line: v.line,
            usedSize: v.usedSize || 0,
          })
        } else {
          res.push({
            name: v.name,
            tls: 0,
            gbl: v.size,
            file: v.sourceFile,
            line: v.line,
            usedSize: v.usedSize || 0,
          })
        }
      }
    }
  }

  return res
}

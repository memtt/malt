import type { ExportChartData } from '@/types/timeline'

/**
 * Composable for exporting timeline chart data in various formats
 */
export function useTimelineExport() {
  /**
   * Export chart data as JSON string
   */
  const exportToJSON = (data: ExportChartData): string => {
    return JSON.stringify(data, null, 2)
  }

  /**
   * Export chart data as CSV string
   * Format: "Series1 timestamp" TAB "Series1" TAB "Series2 timestamp" TAB "Series2" ...
   */
  const exportToCSV = (data: ExportChartData): string => {
    let buffer = ''

    // Header row
    for (let i = 0; i < data.labels.length; i++) {
      if (i > 0) buffer += '\t'
      buffer += `"${data.labels[i]} timestamp"\t"${data.labels[i]}"`
    }
    buffer += '\n'

    // Data rows
    const maxRows = Math.max(...data.data.map((series) => series.length))
    for (let row = 0; row < maxRows; row++) {
      for (let col = 0; col < data.data.length; col++) {
        if (col > 0) buffer += '\t'

        const point = data.data[col][row]
        if (point !== undefined) {
          buffer += `${point.x}\t${point.y}`
        } else {
          buffer += '\t'
        }
      }
      buffer += '\n'
    }

    return buffer
  }

  /**
   * Download JSON data as a file
   * @param data - Chart data to export
   * @param filename - Name for the downloaded file
   */
  const downloadJSON = (data: ExportChartData, filename: string): void => {
    const jsonString = exportToJSON(data)
    const blob = new Blob([jsonString], { type: 'application/json' })
    const url = URL.createObjectURL(blob)

    const link = document.createElement('a')
    link.href = url
    link.download = `${filename}.json`
    document.body.appendChild(link)
    link.click()

    document.body.removeChild(link)
    URL.revokeObjectURL(url)
  }

  /**
   * Download CSV data as a file
   * @param data - Chart data to export
   * @param filename - Name for the downloaded file
   */
  const downloadCSV = (data: ExportChartData, filename: string): void => {
    const csvString = exportToCSV(data)
    const blob = new Blob([csvString], { type: 'text/csv' })
    const url = URL.createObjectURL(blob)

    const link = document.createElement('a')
    link.href = url
    link.download = `${filename}.csv`
    document.body.appendChild(link)
    link.click()

    document.body.removeChild(link)
    URL.revokeObjectURL(url)
  }

  /**
   * Export SVG element to file
   * @param svgElement - The SVG DOM element to export
   * @param filename - Name for the downloaded file
   */
  const exportToSVG = (svgElement: SVGSVGElement, filename: string): void => {
    // Serialize SVG to string
    const svgXml = new XMLSerializer().serializeToString(svgElement)

    // Create blob and download
    const blob = new Blob([svgXml], { type: 'image/svg+xml' })
    const url = URL.createObjectURL(blob)

    // Create temporary link and trigger download
    const link = document.createElement('a')
    link.href = url
    link.download = `${filename}.svg`
    document.body.appendChild(link)
    link.click()

    // Cleanup
    document.body.removeChild(link)
    URL.revokeObjectURL(url)
  }

  /**
   * Show data in a modal (for JSON/CSV preview)
   * Returns the formatted data string
   */
  const prepareModalData = (data: ExportChartData, format: 'json' | 'csv'): string => {
    return format === 'json' ? exportToJSON(data) : exportToCSV(data)
  }

  return {
    exportToJSON,
    exportToCSV,
    exportToSVG,
    downloadJSON,
    downloadCSV,
    prepareModalData,
  }
}

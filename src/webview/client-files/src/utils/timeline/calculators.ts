/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/utils/timeline/calculators.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
/**
 * Calculators for timeline data processing
 */

import type { ChartDataPoint } from '@/types/timeline'

/**
 * Calculate rate from raw values
 * Converts accumulated values to rate per second (e.g., B/s or op/s)
 * @param value - Raw value (bytes or operation count)
 * @param scale - Time scale for the value
 * @param ticksPerSecond - Conversion factor
 * @returns Rate per second
 */
export function calculateRate(value: number, scale: number, ticksPerSecond: number): number {
  if (scale === 0 || ticksPerSecond === 0) return 0
  return value / (scale / ticksPerSecond)
}

/**
 * Add an initial (0, 0) point to data for better visualization
 * @param data - Array of data points
 * @returns New array with initial point prepended
 */
export function addInitialPoint(data: ChartDataPoint[]): ChartDataPoint[] {
  return [{ x: 0, y: 0 }, ...data]
}

/**
 * Interpolate timestamps for data points based on scale
 * @param values - Array of Y values
 * @param perPoints - Scale per point (in ticks)
 * @param startIndex - Starting index (default: 0)
 * @returns Array of data points with interpolated X values
 */
export function interpolateTimestamps(
  values: number[],
  perPoints: number,
  startIndex: number = 0,
): ChartDataPoint[] {
  return values.map((value, i) => ({
    x: (i + startIndex + 1) * perPoints,
    y: value,
  }))
}

/**
 * Extract specific column from 2D array
 * @param data - 2D array of values
 * @param columnIndex - Index of the column to extract
 * @returns Array of values from the specified column
 */
export function extractColumn(data: number[][], columnIndex: number): number[] {
  return data.map((row) => row[columnIndex] || 0)
}

/**
 * Calculate moving average for smoothing data
 * @param data - Array of data points
 * @param windowSize - Size of the moving average window
 * @returns Array of smoothed data points
 */
export function movingAverage(data: ChartDataPoint[], windowSize: number): ChartDataPoint[] {
  if (windowSize <= 1 || data.length < windowSize) return data

  const result: ChartDataPoint[] = []
  const halfWindow = Math.floor(windowSize / 2)

  for (let i = 0; i < data.length; i++) {
    const start = Math.max(0, i - halfWindow)
    const end = Math.min(data.length, i + halfWindow + 1)
    const window = data.slice(start, end)

    const sum = window.reduce((acc, point) => acc + point.y, 0)
    const avg = sum / window.length

    result.push({ x: data[i].x, y: avg })
  }

  return result
}

/**
 * Find the maximum value in a series
 * @param data - Array of data points
 * @returns Maximum Y value
 */
export function findMaxValue(data: ChartDataPoint[]): number {
  if (data.length === 0) return 0
  return Math.max(...data.map((point) => point.y))
}

/**
 * Find the minimum value in a series
 * @param data - Array of data points
 * @returns Minimum Y value
 */
export function findMinValue(data: ChartDataPoint[]): number {
  if (data.length === 0) return 0
  return Math.min(...data.map((point) => point.y))
}

/**
 * Calculate the average value of a series
 * @param data - Array of data points
 * @returns Average Y value
 */
export function calculateAverage(data: ChartDataPoint[]): number {
  if (data.length === 0) return 0
  const sum = data.reduce((acc, point) => acc + point.y, 0)
  return sum / data.length
}

/**
 * Find the closest data point to a given X value
 * @param data - Array of data points
 * @param xValue - Target X value
 * @returns Closest data point
 */
export function findClosestPoint(data: ChartDataPoint[], xValue: number): ChartDataPoint | null {
  if (data.length === 0) return null

  let closestPoint = data[0]
  let minDistance = Math.abs(data[0].x - xValue)

  for (const point of data) {
    const distance = Math.abs(point.x - xValue)
    if (distance < minDistance) {
      minDistance = distance
      closestPoint = point
    }
  }

  return closestPoint
}

/**
 * Calculate time duration from start to end
 * @param startTicks - Start time in ticks
 * @param endTicks - End time in ticks
 * @param ticksPerSecond - Conversion factor
 * @returns Duration in seconds
 */
export function calculateDuration(
  startTicks: number,
  endTicks: number,
  ticksPerSecond: number,
): number {
  return (endTicks - startTicks) / ticksPerSecond
}

/**
 * Downsample data for better performance with large datasets
 * Uses Largest-Triangle-Three-Buckets (LTTB) algorithm simplified version
 * @param data - Array of data points
 * @param targetSize - Target number of points
 * @returns Downsampled array
 */
export function downsampleData(data: ChartDataPoint[], targetSize: number): ChartDataPoint[] {
  if (data.length <= targetSize) return data

  const result: ChartDataPoint[] = []
  const bucketSize = (data.length - 2) / (targetSize - 2)

  // Always include first point
  result.push(data[0])

  for (let i = 0; i < targetSize - 2; i++) {
    const bucketStart = Math.floor(i * bucketSize) + 1
    const bucketEnd = Math.floor((i + 1) * bucketSize) + 1

    // Find point with highest Y value in bucket (simple approach)
    let maxPoint = data[bucketStart]
    for (let j = bucketStart; j < bucketEnd && j < data.length; j++) {
      if (data[j].y > maxPoint.y) {
        maxPoint = data[j]
      }
    }

    result.push(maxPoint)
  }

  // Always include last point
  result.push(data[data.length - 1])

  return result
}

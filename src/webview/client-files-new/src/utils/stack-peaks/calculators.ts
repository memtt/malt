/**
 * Calculators for stack peaks data
 */

import type { StackInfo } from '@/types/stack-peaks'

/**
 * Find the largest stack size across all threads
 * @param stacks - Array of stack info
 * @returns Maximum stack size in bytes
 */
export function findLargestStack(stacks: StackInfo[]): number {
  if (stacks.length === 0) return 0

  let largest = 0
  for (const stack of stacks) {
    if (stack.size > largest) {
      largest = stack.size
    }
  }

  return largest
}

/**
 * Calculate dynamic height for charts based on number of items
 * @param itemCount - Number of items (threads or functions)
 * @param baseHeight - Base height for margins and axes (default: 150)
 * @param itemHeight - Height per item (default: 16)
 * @returns Total height in pixels
 */
export function calculateDynamicHeight(
  itemCount: number,
  baseHeight: number = 150,
  itemHeight: number = 16,
): number {
  return baseHeight + itemCount * itemHeight
}

/**
 * Calculate average stack size for a thread
 * @param stack - Stack info for a thread
 * @returns Average stack size in bytes
 */
export function calculateAverageStackSize(stack: StackInfo): number {
  if (stack.timeprofiler.max.length === 0) return 0

  const sum = stack.timeprofiler.max.reduce((acc, val) => acc + val, 0)
  return sum / stack.timeprofiler.max.length
}

/**
 * Calculate peak stack size for a thread
 * @param stack - Stack info for a thread
 * @returns Peak stack size in bytes
 */
export function calculatePeakStackSize(stack: StackInfo): number {
  return stack.size
}

/**
 * Find thread with largest stack
 * @param stacks - Array of stack info
 * @returns Thread ID (index) with largest stack, or -1 if none
 */
export function findThreadWithLargestStack(stacks: StackInfo[]): number {
  if (stacks.length === 0) return -1

  let largestIndex = 0
  let largestSize = stacks[0].size

  for (let i = 1; i < stacks.length; i++) {
    if (stacks[i].size > largestSize) {
      largestSize = stacks[i].size
      largestIndex = i
    }
  }

  return largestIndex
}

/**
 * Calculate total memory used by all function frames
 * @param details - Array of stack details
 * @returns Total memory in bytes
 */
export function calculateTotalFrameMemory(details: { mem: number }[]): number {
  return details.reduce((sum, detail) => sum + detail.mem, 0)
}

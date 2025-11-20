/**
 * Service for fetching flat function statistics
 */

import { request } from '@/lib/request'
import type { FlatFunction } from '@/types/flat'

/**
 * Fetch flat function statistics
 */
export async function fetchFlatFunctions(): Promise<FlatFunction[]> {
  return request<FlatFunction[]>('/flat.json', 'GET')
}

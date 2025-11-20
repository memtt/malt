/**
 * Home API Service
 * Handles API calls for home dashboard summary data
 */

import { request } from '@/lib/request';
import type { HomeSummaryData } from '@/types/home';

/**
 * Fetch home summary data
 * @returns Promise with home summary data
 */
export async function fetchHomeSummary(): Promise<HomeSummaryData> {
  const data = await request<HomeSummaryData>('/data/summary.json', 'GET');
  return data;
}

import type { TimelineData, ActiveChunksResponse } from '@/types/timeline'
import { request } from '@/lib/request'

/**
 * Service for fetching timeline data
 */
export const timelineService = {
  /**
   * Fetch timeline data from the server
   * Contains memory, system, and bandwidth data over time
   */
  getTimelineData: async (): Promise<TimelineData> => {
    return request<TimelineData>('/timed.json', 'GET')
  },

  /**
   * Fetch active memory chunks at a specific timestamp
   * @param timestamp - Timestamp in ticks
   * @returns List of allocated chunks with their call stacks
   */
  getActiveChunks: async (timestamp: number): Promise<ActiveChunksResponse> => {
    // Note: The actual endpoint may vary based on backend implementation
    // Angular code used a dynamic endpoint or POST request
    return request<ActiveChunksResponse>(`/data/active-chunks-at-${timestamp}.json`, 'GET')
  },
}

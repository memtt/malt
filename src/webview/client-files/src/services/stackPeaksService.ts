/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/services/stackPeaksService.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
import type { StackPeaksData, StackDetailsData } from '@/types/stack-peaks'
import { request } from '@/lib/request'

/**
 * Service for fetching stack peaks data
 */
export const stackPeaksService = {
  /**
   * Fetch stack peaks data from the server
   * Contains stack memory information for all threads
   * @returns Stack peaks data with timeline information
   */
  getStackPeaksData: async (): Promise<StackPeaksData> => {
    return request<StackPeaksData>('/stacks-mem.json', 'GET')
  },

  /**
   * Fetch detailed stack information for a specific thread
   * @param threadId - Thread ID (0-based index)
   * @returns Stack details with function-level memory usage
   */
  getStackDetails: async (threadId: number): Promise<StackDetailsData> => {
    return request<StackDetailsData>(`/stack.json?id=${threadId}`, 'GET')
  },
}

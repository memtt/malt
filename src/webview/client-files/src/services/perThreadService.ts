/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/services/perThreadService.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
import type { PerThreadData } from '@/types/per-thread'
import { request } from '@/lib/request'

/**
 * Service for fetching per-thread data
 * Uses the global summary endpoint which contains thread statistics
 */
export const perThreadService = {
  /**
   * Fetch summary data from the server
   * Contains thread statistics for all allocation functions
   * @returns Per-thread data with system info and thread statistics
   */
  getPerThreadData: async (): Promise<PerThreadData> => {
    return request<PerThreadData>('/data/summary.json', 'GET')
  },
}

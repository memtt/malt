/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/services/flatService.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
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

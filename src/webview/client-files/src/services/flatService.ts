/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 06/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/services/flatService.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
*    AUTHOR   : Sébastien Valat (ISTerre & IPAG / UGA / CNRS) - 2026
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
  return request<FlatFunction[]>('./flat.json', 'GET')
}

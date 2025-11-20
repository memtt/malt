/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/services/reallocService.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
/**
 * Realloc API Service
 * Handles API calls for realloc data with Zod validation
 */

import { request } from '@/lib/request';
import { reallocDataSchema, type ReallocData } from '@/types/realloc';

export async function fetchRealloc(): Promise<ReallocData> {
  const data = await request<unknown>('/realloc-map.json', 'GET');
  return reallocDataSchema.parse(data);
}

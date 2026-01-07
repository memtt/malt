/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/services/globalVarsService.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
/**
 * Global Variables API Service
 * Handles API calls for global variables data with Zod validation
 */

import { request } from '@/lib/request';
import { globalVarsDataSchema, type GlobalVarsData } from '@/types/global-vars';

export async function fetchGlobalVars(): Promise<GlobalVarsData> {
  const data = await request<unknown>('/global-variables.json', 'GET');
  return globalVarsDataSchema.parse(data);
}

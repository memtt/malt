/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/types/realloc.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
/**
 * Types for Realloc analysis
 */

import { z } from 'zod';

// Zod schemas
export const reallocEntrySchema = z.object({
  oldSize: z.number(),
  newSize: z.number(),
  count: z.number(),
});

export const reallocDataSchema = z.array(reallocEntrySchema);

// Export types inferred from schemas
export type ReallocEntry = z.infer<typeof reallocEntrySchema>;
export type ReallocData = z.infer<typeof reallocDataSchema>;

// Computed data types
export interface DeltaDistribution {
  [delta: number]: number;
}

export interface Log2HistoEntry {
  sizeLog: number;
  count: number;
  label: string;
}

export interface MostUsedEntry extends ReallocEntry {
  label: string;
}

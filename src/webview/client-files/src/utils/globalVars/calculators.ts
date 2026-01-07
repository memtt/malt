/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/utils/globalVars/calculators.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
/**
 * Global Variables Calculators
 * Pure functions for calculations
 */

import type { GlobalVarsData, GlobalVariable } from '@/types/global-vars';

export interface Summary {
  global: number;
  tls: number;
}

export interface TopVariable extends GlobalVariable {
  binaryFile: string;
  totalSize: number; // Calculated size (size * maxThreadCount for TLS, otherwise size)
}

/**
 * Calculate summary of global and TLS variables
 */
export function calculateSummary(data: GlobalVarsData | null, tlsInstances: number): Summary {
  if (!data) return { global: 0, tls: 0 };

  let tls = 0;
  let gbl = 0;

  for (const bin in data.vars) {
    for (const v of data.vars[bin]) {
      if (v.tls) {
        tls += v.size;
      } else {
        gbl += v.size;
      }
    }
  }

  return {
    global: gbl,
    tls: tls * tlsInstances
  };
}

/**
 * Deduplicate variables: keep only one if name AND size are identical
 * If name is same but size differs, keep all variants
 */
function deduplicateVariables(variables: TopVariable[]): TopVariable[] {
  const seen = new Map<string, TopVariable>();

  for (const variable of variables) {
    // Create unique key: name + size + tls
    const key = `${variable.name}-${variable.size}-${variable.tls}`;

    if (!seen.has(key)) {
      // First occurrence of this name+size combination
      seen.set(key, variable);
    }
    // If already seen with same name AND size, skip (duplicate)
  }

  return Array.from(seen.values());
}

/**
 * Get all global variables sorted by total size
 * Returns all variables (not limited) for search functionality
 * Deduplicates variables with identical name AND size
 */
export function getAllVariables(
  data: GlobalVarsData | null,
  maxThreadCount: number
): TopVariable[] {
  if (!data) return [];

  const allVariables: TopVariable[] = [];

  // Flatten all variables from all binaries
  for (const binaryFile in data.vars) {
    for (const variable of data.vars[binaryFile]) {
      const totalSize = variable.tls ? variable.size * maxThreadCount : variable.size;
      allVariables.push({
        ...variable,
        binaryFile,
        totalSize,
      });
    }
  }

  // Deduplicate before sorting
  const deduplicated = deduplicateVariables(allVariables);

  // Sort by totalSize descending
  return deduplicated.sort((a, b) => b.totalSize - a.totalSize);
}

/**
 * Get top N global variables sorted by total size
 */
export function getTopVariables(
  data: GlobalVarsData | null,
  maxThreadCount: number,
  limit = 10
): TopVariable[] {
  return getAllVariables(data, maxThreadCount).slice(0, limit);
}

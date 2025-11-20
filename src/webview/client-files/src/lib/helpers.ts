/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/lib/helpers.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
/**
 * Helper utilities for MALT
 * Ported from the Angular implementation
 */

/**
 * Convert a value to human readable format with units
 * @param value - The numeric value to format
 * @param digits - Number of decimal places
 * @param unit - The unit to use (B for bytes, % for percent, etc.)
 * @param appendUnit - Whether to append the unit to the result
 * @returns Formatted string with appropriate unit prefix
 */
export function humanReadable(
  value: number,
  digits: number = 1,
  unit: string = 'B',
  appendUnit: boolean = true
): string {
  if (value === 0) return `0${appendUnit && unit ? ' ' + unit : ''}`;

  // Use 1024 for bytes, 1000 for everything else
  const divisor = unit === 'B' ? 1024 : 1000;

  const prefixes = ['', 'K', 'M', 'G', 'T', 'P'];
  let prefixIndex = 0;
  let adjustedValue = Math.abs(value);

  // Adjust decimals: if value is between 1 and divisor, use 0 decimals
  let effectiveDigits = digits;
  if (adjustedValue >= 1 && adjustedValue < divisor) {
    effectiveDigits = 0;
  }

  while (adjustedValue >= divisor && prefixIndex < prefixes.length - 1) {
    adjustedValue /= divisor;
    prefixIndex++;
  }

  const sign = value < 0 ? '-' : '';
  const formatted = adjustedValue.toFixed(effectiveDigits);
  const prefix = prefixes[prefixIndex];

  if (appendUnit) {
    // Add unit after prefix (KB, MB, etc. for bytes; K, M for counts)
    const fullUnit = prefix ? `${prefix}${unit}` : unit;
    return `${sign}${formatted} ${fullUnit}`;
  } else {
    return `${sign}${formatted}${prefix}`;
  }
}

/**
 * Sort data by value in descending order
 */
export function sortByValue<T extends { value: number }>(data: T[]): T[] {
  return [...data].sort((a, b) => b.value - a.value);
}

/**
 * Cut data for pie charts to show only top N entries
 * @param data - Array of data with name and value
 * @param curPercent - Percentage threshold for cumulative display
 * @param maxNb - Maximum number of entries to show
 * @param ignorePercent - Percentage threshold to ignore "others"
 */
export function cutForPie<T extends { name: string; value: number }>(
  data: T[],
  curPercent: number = 95,
  maxNb: number = 8,
  _ignorePercent: number = 0.1
): Array<T | { name: string; value: number }> {
  // Compute total
  const tot = data.reduce((sum, item) => sum + item.value, 0);

  // Sort by value descending
  const sortedRes = sortByValue(data);

  // Search first N% or maxNb items
  let cum = 0;
  const finalRes: Array<T | { name: string; value: number }> = [];

  for (let i = 0; i < sortedRes.length; i++) {
    cum += sortedRes[i].value;
    if (((100 * cum) / tot > curPercent && sortedRes.length - i >= 3) || i >= maxNb) {
      finalRes.push({ name: 'others', value: tot - cum });
      break;
    } else {
      finalRes.push(sortedRes[i]);
    }
  }

  return finalRes;
}

/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/utils/globalVars/formatters.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
/**
 * Global Variables Chart Formatters
 * Pure functions for formatting chart data
 */

import type { BarSeries, BarValue } from '@/types/global-vars';
import type { BarChartRawData } from './transformers';

/**
 * Cut data for bar chart (limit to top entries at 95%)
 */
export function cutDataForBarChart<T extends { tls: number; gbl: number }>(
  vars: T[]
): T[] {
  if (vars.length < 16) return vars;

  // Sort by total value
  const sorted = [...vars].sort((a, b) => b.tls + b.gbl - (a.tls + a.gbl));

  // Calculate totals
  let tot = 0;
  let _totTLS = 0;
  let _totGLB = 0;
  for (const v of sorted) {
    tot += v.tls + v.gbl;
    _totTLS += v.tls;
    _totGLB += v.gbl;
  }

  // Cut at 95%
  const res: T[] = [];
  let cur = 0;
  let curTLS = 0;
  let curGLB = 0;
  let cnt = 0;

  for (const v of sorted) {
    if ((100 * cur) / tot < 95 && cnt < 300) {
      cnt++;
      cur += v.tls + v.gbl;
      res.push(v);
    } else {
      curTLS += v.tls;
      curGLB += v.gbl;
    }
  }

  if (curTLS > 0 || curGLB > 0) {
    res.push({ name: 'OTHERS', tls: curTLS, gbl: curGLB, usedSize: 0 } as any as T);
  }

  return res;
}

/**
 * Format data for bar chart with separate series
 */
export function formatDataForBarChart(
  vars: BarChartRawData[],
  tlsInstances: number
): BarSeries[] {
  const gbl: BarValue[] = [];
  const tls: BarValue[] = [];
  let sum = 0;

  // Sort by total value
  const varsSorted = [...vars].sort((a, b) => b.tls + b.gbl - (a.tls + a.gbl));

  // Split into two series
  for (const v of varsSorted) {
    sum += v.gbl + v.tls;
    gbl.push({
      name: v.name,
      value: v.gbl,
      file: v.file,
      line: v.line,
      usedSize: v.usedSize || 0
    });
    tls.push({
      name: v.name,
      value: v.tls,
      file: v.file,
      line: v.line,
      usedSize: v.usedSize || 0
    });
  }

  return [
    {
      key: 'Global variables',
      color: '#0d6efd',
      values: gbl,
      total: sum
    },
    {
      key: 'TLS variables',
      color: '#dc3545',
      values: tls,
      tlsInstances: tlsInstances,
      total: sum
    }
  ];
}

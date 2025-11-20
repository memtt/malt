/**
 * Metric definitions for function profiling
 * Ported from Angular func-metrics.js
 */

import type { MetricKey, MetricOrder, StackInfoData } from '@/types/sources';

export interface MetricDefinition {
  name: string;
  key: MetricKey;
  extractor: (data: StackInfoData) => number;
  formatter: (value: number) => string;
  defaultOrder: MetricOrder;
  ref: 'sum' | 'max'; // Reference type for ratio calculation
}

export interface MetricListItem {
  name: string;
  key: MetricKey;
}

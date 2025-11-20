/**
 * Types for Global Variables analysis
 */

import { z } from 'zod';

// Zod schemas
export const globalVariableSchema = z.object({
  name: z.string(),
  size: z.number(),
  tls: z.boolean(),
  sourceFile: z.string().optional(),
  line: z.number().optional(),
  usedSize: z.number().optional(),
});

export const globalVarsDataSchema = z.object({
  maxThreadCount: z.number(),
  vars: z.record(z.string(), z.array(globalVariableSchema)),
});

// Export types inferred from schemas
export type GlobalVariable = z.infer<typeof globalVariableSchema>;
export type GlobalVarsData = z.infer<typeof globalVarsDataSchema>;

export const pieDataSchema = z.object({
  name: z.string(),
  value: z.number(),
  file: z.string().optional(),
  line: z.number().optional(),
  usedSize: z.number().optional(),
});

export const barValueSchema = z.object({
  name: z.string(),
  value: z.number(),
  file: z.string().optional(),
  line: z.number().optional(),
  usedSize: z.number().optional(),
});

export const barSeriesSchema = z.object({
  key: z.string(),
  color: z.string(),
  values: z.array(barValueSchema),
  total: z.number().optional(),
  tlsInstances: z.number().optional(),
});

// Export types inferred from schemas
export type PieData = z.infer<typeof pieDataSchema>;
export type BarValue = z.infer<typeof barValueSchema>;
export type BarSeries = z.infer<typeof barSeriesSchema>;

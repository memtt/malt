/**
 * Sources API Service
 * Handles API calls for source code viewing and analysis
 */

import { request } from '@/lib/request';
import {
  functionStatsArraySchema,
  sourceAnnotationsArraySchema,
  callStackArraySchema,
  type FunctionStat,
  type SourceAnnotation,
  type CallStackEntry,
  type CallStackFilter,
} from '@/types/sources';

/**
 * Load source code file content
 * @param filePath - Path to the source file
 * @returns Raw source code as string
 */
export async function fetchSourceFile(filePath: string): Promise<string> {
  return request<string>('/source-file', 'POST', { path: filePath }, { responseType: 'text' });
}

/**
 * Load annotations (profiling data) for a source file
 * @param filePath - Path to the source file
 * @returns Array of per-line annotations
 */
export async function fetchFileAnnotations(
  filePath: string,
): Promise<SourceAnnotation[]> {
  const data = await request<unknown>('/file-infos.json', 'POST', { file: filePath });
  return sourceAnnotationsArraySchema.parse(data);
}

/**
 * Load flat function statistics (function list)
 * @returns Array of function statistics
 */
export async function fetchFlatFunctions(): Promise<FunctionStat[]> {
  const data = await request<unknown>('/flat.json', 'GET');
  return functionStatsArraySchema.parse(data);
}

/**
 * Load call stack tree level (lazy loading)
 * @param parentStackId - Parent stack ID (0 for root)
 * @param parentStackDepth - Parent stack depth (0 for root)
 * @param filter - Filter by file/line/function
 * @returns Array of call stack entries for this level
 */
export async function fetchCallStackLevel(
  parentStackId: number,
  parentStackDepth: number,
  filter: CallStackFilter,
): Promise<CallStackEntry[]> {
  const data = await request<unknown>('/call-stack-next-level.json', 'POST', {
    parentStackId,
    parentStackDepth,
    filter,
  });
  return callStackArraySchema.parse(data);
}

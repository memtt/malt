/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/services/callTreeService.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/
/**
 * Call Tree API Service
 * Handles API calls for call tree graph data
 */

import { request } from '@/lib/request';
import { API_URL } from '@/config/app';
import type {
  CallTreeData,
  CallTreeParams,
  CallTreeDownloadFormat,
} from '@/types/call-tree';
import { boolean } from 'zod';

/**
 * Fetch call tree graph data
 * @param params - Call tree request parameters
 * @returns Promise with call tree data
 */
export async function fetchCallTree(params: CallTreeParams): Promise<CallTreeData> {
  const data = await request<CallTreeData>('/calltree', 'POST', {
    func: params.func,
    nodeid: params.nodeid,
    depth: params.depth,
    height: params.height,
    mincost: params.mincost,
    metric: params.metric,
    isratio: params.isratio,
  });

  return data;
}

/**
 * Get download link for call tree export
 * @param params - Call tree parameters
 * @param format - Export format (svg or dot)
 * @returns Download URL
 */
export function getCallTreeDownloadLink(
  params: CallTreeParams,
  format: CallTreeDownloadFormat,
): string {
  const queryParams = new URLSearchParams({
    ...(params.func ? { func: params.func } : { nodeid: String(params.nodeid ?? -1) }),
    depth: String(params.depth),
    height: String(params.height),
    mincost: String(params.mincost),
    metric: params.metric,
    isratio: String(params.isratio),
    format: format,
  });

  return `${API_URL}/calltree?${queryParams.toString()}`;
}

/**
 * Download call tree file with authentication (SVG or DOT)
 * Fetches the file with Bearer token, then triggers browser download
 * @param params - Call tree parameters
 * @param format - Export format (svg or dot)
 * @param filename - Name for the downloaded file
 */
export async function downloadCallTreeFile(
  params: CallTreeParams,
  format: CallTreeDownloadFormat,
  filename: string,
): Promise<void> {
  // Import authStore here to avoid circular dependency
  const { useAuthStore } = await import('@/stores/authStore');
  const authStore = useAuthStore();
  const token = authStore.getToken();

  // Build URL
  const url = getCallTreeDownloadLink(params, format);

  // Fetch with authentication
  const response = await fetch(url, {
    method: 'POST',
    headers: (token ? { Authorization: `Bearer ${token}` } : {}),
    body: JSON.stringify({
      func: params.func ?? null,
      nodeid: params.nodeid ?? -1,
      depth: params.depth,
      height: params.height,
      mincost: params.mincost,
      metric: params.metric,
      isratio: params.isratio,
      format: format,
    }),
  });

  if (!response.ok) {
    throw new Error(`Failed to download ${format}: ${response.statusText}`);
  }

  // Get content as blob
  const response_json = await response.json();

  //key
  var key: string = "svg";
  var mimetype: string = "image/svg+xml";
  if (format == "dot") {
    key = "dotCode";
    mimetype = "text/vnd.graphviz";
  }

  // Create temporary URL and trigger download
  const blobUrl = URL.createObjectURL(new Blob([response_json[key]], {type: mimetype}));
  const link = document.createElement('a');
  link.href = blobUrl;
  link.download = filename;
  document.body.appendChild(link);
  link.click();

  // Cleanup
  document.body.removeChild(link);
  URL.revokeObjectURL(blobUrl);
}

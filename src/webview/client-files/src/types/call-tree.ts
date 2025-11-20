/**
 * Call Tree Types
 * Types for the call tree graph visualization and navigation
 */

/**
 * Call tree error types
 */
export interface CallTreeError {
  svgGenerationError?: string;
  filterError?: string;
  networkError?: string;
  nodeNotFoundError?: boolean;
}

/**
 * Call tree data response from backend
 */
export interface CallTreeData {
  /** SVG content as string */
  svg: string;
  /** Graphviz DOT code */
  dotCode: string;
  /** Current node ID */
  nodeId: number;
  /** Function name */
  function: string;
  /** Function name (short version) */
  functionShort?: string;
  /** File path */
  file: string;
  /** File path (short version) */
  fileShort?: string;
  /** Number of visible nodes in the graph */
  visibleNodes: number;
  /** Total number of nodes */
  totalNodes: number;
  /** Error information if any */
  error?: CallTreeError;
}

/**
 * Call tree filter parameters
 */
export interface CallTreeFilters {
  /** Maximum height from selected node (upward) */
  height: number;
  /** Maximum depth from selected node (downward) */
  depth: number;
  /** Minimum node cost percentage to display */
  nodeCost: number;
}

/**
 * Call tree request parameters
 */
export interface CallTreeParams {
  /** Node ID to center the graph on (null when searching by function name) */
  nodeid?: number | null;
  /** Function name to search for (null if not searching by function) */
  func?: string | null;
  /** Maximum depth */
  depth: number;
  /** Maximum height */
  height: number;
  /** Minimum cost percentage */
  mincost: number;
  /** Selected metric */
  metric: string;
  /** Whether to use ratio mode */
  isratio: boolean;
}

/**
 * Navigation history state
 */
export interface CallTreeNavigationState {
  /** Node ID (null when searching by function name) */
  nodeId: number | null;
  /** Optional function name */
  function?: string;
}

/**
 * Download format options
 */
export type CallTreeDownloadFormat = 'svg' | 'dot';

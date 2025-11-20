/**
 * HTML escaping utilities for safe rendering
 * Prevents XSS when displaying source code
 */

const TAGS_TO_REPLACE: Record<string, string> = {
  '&': '&amp;',
  '<': '&lt;',
  '>': '&gt;',
};

/**
 * Escape HTML special characters
 * @param str - String to escape
 * @returns Escaped string safe for HTML
 */
export function escapeHtml(str: string): string {
  return str.replace(/[&<>]/g, (tag) => TAGS_TO_REPLACE[tag] || tag);
}

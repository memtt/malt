/**
 * Prism.js Code Annotator Plugin
 * Adds inline annotations to code lines with click handlers
 * Ported from Angular Prism plugin (prism.js custom plugin)
 */

export interface AnnotationData {
  line: number;
  text: string;
  color?: string;
  onClick?: (element: HTMLElement, data: AnnotationData) => void;
  data?: any;
}

interface AnnotationEntry {
  element: HTMLAnchorElement;
  data: AnnotationData;
}

// Store annotations per pre element
const annotationsMap = new WeakMap<HTMLElement, AnnotationEntry[]>();

// Store last highlight element
const lastHighlightMap = new WeakMap<HTMLElement, HTMLDivElement>();

/**
 * Check if line height is rounded
 */
function isLineHeightRounded(): boolean {
  const pre = document.createElement('pre');
  pre.style.lineHeight = '1.5';
  document.body.appendChild(pre);
  const isRounded = parseInt(getComputedStyle(pre).lineHeight) % 1 === 0;
  document.body.removeChild(pre);
  return isRounded;
}

/**
 * Check if element has class
 */
function hasClass(element: HTMLElement, className: string): boolean {
  return element.classList.contains(className);
}

/**
 * Click event handler for annotations
 */
function clickEventHandler(this: HTMLAnchorElement, event: Event) {
  event.preventDefault();

  const preElement = this.parentNode as HTMLElement;
  if (!preElement) return;

  const annotations = annotationsMap.get(preElement);
  if (!annotations) return;

  for (const annotation of annotations) {
    if (this === annotation.element && annotation.data.onClick) {
      annotation.data.onClick(this, annotation.data);
      break;
    }
  }
}

/**
 * Add annotation to a specific line
 */
export function addAnnotation(
  codeElement: HTMLElement,
  annotation: AnnotationData,
): void {
  const preElement = codeElement.parentNode as HTMLElement;

  if (!preElement || !annotation || !/pre/i.test(preElement.nodeName)) {
    console.warn('[CodeAnnotator] Invalid preElement or annotation', { preElement, annotation });
    return;
  }

  const offset = +(preElement.getAttribute('data-line-offset') || '0');
  const parseMethod = isLineHeightRounded() ? parseInt : parseFloat;
  const lineHeight = parseMethod(getComputedStyle(preElement).lineHeight);

  // Create annotation link element
  const link = document.createElement('a');
  link.href = 'javascript:;';
  link.innerHTML = `<span>${annotation.text}</span>`;
  link.className = 'line-annotate';
  link.style.top = `${(annotation.line - offset - 1) * lineHeight}px`;

  if (annotation.color) {
    link.style.backgroundColor = annotation.color;
  }

  if (annotation.onClick) {
    link.addEventListener('click', clickEventHandler);
  }

  preElement.appendChild(link);

  // Store annotation
  const annotations = annotationsMap.get(preElement) || [];
  annotations.push({ element: link, data: annotation });
  annotationsMap.set(preElement, annotations);
}

/**
 * Remove all annotations from code element
 */
export function removeAllAnnotations(codeElement: HTMLElement): void {
  if (!codeElement) return;

  const preElement = codeElement.parentNode as HTMLElement;
  if (!preElement) return;

  const annotations = annotationsMap.get(preElement);
  if (!annotations) return;

  // Remove all annotation elements
  for (const annotation of annotations) {
    if (annotation.data.onClick) {
      annotation.element.removeEventListener('click', clickEventHandler);
    }
    annotation.element.remove();
  }

  annotationsMap.delete(preElement);
}

/**
 * Scroll to a specific line
 */
function scrollTo(codeElement: HTMLElement, lineNo: number): void {
  const preElement = codeElement.parentNode as HTMLElement;
  if (!preElement) return;

  const offset = +(preElement.getAttribute('data-line-offset') || '0');
  const parseMethod = isLineHeightRounded() ? parseInt : parseFloat;
  const lineHeight = parseMethod(getComputedStyle(preElement).lineHeight);
  const preScroll = (lineNo - offset - 1) * lineHeight;

  // Find the scrollable container
  const editorContainer = preElement.parentElement;
  if (editorContainer) {
    editorContainer.scrollTop = preScroll - 100; // Offset 100px from top for better visibility
  } else {
    preElement.scrollTop = preScroll;
  }
}

/**
 * Highlight a specific line
 */
function highlight(codeElement: HTMLElement, lineNo: number): void {
  removeHighlight(codeElement);

  const preElement = codeElement.parentNode as HTMLElement;
  if (!preElement) return;

  const parseMethod = isLineHeightRounded() ? parseInt : parseFloat;
  const lineHeight = parseMethod(getComputedStyle(preElement).lineHeight);

  // Create highlight div with INLINE styles to bypass any CSS issues
  const line = document.createElement('div');
  line.textContent = ' ';
  line.className = 'line-highlight';

  // Force inline styles
  line.style.position = 'absolute';
  line.style.left = '0';
  line.style.right = '0';
  line.style.top = `${(lineNo - 1) * lineHeight + 15}px`; // +15px offset for alignment
  line.style.height = `${lineHeight}px`;
  line.style.backgroundColor = 'rgba(255, 243, 205, 0.8)';
  line.style.pointerEvents = 'none';
  line.style.zIndex = '1'; // Behind annotations and code
  line.style.display = 'block';
  line.style.marginTop = '0';
  line.style.padding = '0';

  // Allow this to play nicely with the line-numbers plugin
  if (hasClass(preElement, 'line-numbers')) {
    // Need to attach to pre as when line-numbers is enabled, the code tag is relative which screws up the positioning
    preElement.appendChild(line);
  } else {
    const codeTag = preElement.querySelector('code');
    (codeTag || preElement).appendChild(line);
  }

  lastHighlightMap.set(codeElement, line);

  // Note: Highlight stays visible until removeHighlight() is called
  // (when selecting another line or clicking elsewhere)
}

/**
 * Remove highlight from code element
 */
function removeHighlight(codeElement: HTMLElement): void {
  const lastHighlight = lastHighlightMap.get(codeElement);
  if (lastHighlight) {
    lastHighlight.remove();
    lastHighlightMap.delete(codeElement);
  }
}

/**
 * Scroll to and highlight a specific line
 */
export function scrollToAndHighlight(
  codeElement: HTMLElement,
  line: number,
): void {
  highlight(codeElement, line);
  scrollTo(codeElement, line);
}

/**
 * Initialize plugin (registers with Prism if available)
 */
export function initCodeAnnotator(): void {
  if (typeof window !== 'undefined' && (window as any).Prism) {
    (window as any).Prism.plugins = (window as any).Prism.plugins || {};
    (window as any).Prism.plugins.codeAnnotator = {
      add: addAnnotation,
      removeAll: removeAllAnnotations,
      scrollToAndHighlight,
      scrollTo,
      highlight,
      removeHighlight,
    };
  }
}

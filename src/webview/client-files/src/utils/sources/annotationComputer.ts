/**
 * Annotation computation utilities
 * Ported from Angular source-editor.js
 */

import type {
  SourceAnnotation,
  StackInfoData,
  StackMinMaxInfo,
} from '@/types/sources';

/**
 * Merge two StackMinMaxInfo objects
 * @param onto - Target object to merge into (mutated)
 * @param value - Source object to merge from
 */
export function mergeStackMinMaxInfo(
  onto: StackMinMaxInfo,
  value: StackMinMaxInfo,
): void {
  onto.count += value.count;
  onto.sum += value.sum;
  if (onto.min === 0 || (value.min < onto.min && value.min !== 0)) {
    onto.min = value.min;
  }
  if (onto.max === 0 || (value.max > onto.max && value.max !== 0)) {
    onto.max = value.max;
  }
}

/**
 * Merge two StackInfoData objects
 * @param onto - Target object to merge into (mutated)
 * @param value - Source object to merge from
 */
export function mergeStackInfoData(
  onto: StackInfoData,
  value: StackInfoData,
): void {
  onto.countZeros += value.countZeros;
  onto.maxAliveReq += value.maxAliveReq;
  onto.aliveReq += value.aliveReq;
  mergeStackMinMaxInfo(onto.alloc, value.alloc);
  mergeStackMinMaxInfo(onto.free, value.free);
  mergeStackMinMaxInfo(onto.lifetime, value.lifetime);
}

/**
 * Compute total (own + childs) for an annotation
 * Mutates the annotation object to add 'total' field
 * @param annotation - Source annotation to compute total for
 */
export function computeTotal(annotation: SourceAnnotation): void {
  // Already computed
  if (annotation.total !== undefined) {
    return;
  }

  // Only childs, no own
  if (annotation.own === undefined) {
    annotation.total = structuredClone(annotation.childs);
    return;
  }

  // Copy own
  annotation.total = structuredClone(annotation.own);

  // Merge childs if exists
  if (annotation.childs !== undefined) {
    mergeStackInfoData(annotation.total, annotation.childs);
  }
}

/**
 * Enrich annotations array with computed total and file info
 * @param annotations - Array of annotations
 * @param file - Current file path
 * @returns Enriched annotations array
 */
export function enrichAnnotations(
  annotations: SourceAnnotation[],
  file: string,
): SourceAnnotation[] {
  return annotations.map((annotation) => {
    const enriched = { ...annotation, file };
    computeTotal(enriched);
    return enriched;
  });
}

/**
 * Find line with largest annotation value for a function
 * @param annotations - Array of annotations
 * @param file - File path
 * @param func - Function name
 * @param getValue - Function to extract value from annotation
 * @returns Line number with largest value, or -1 if not found
 */
export function findLargestAnnotationLine(
  annotations: SourceAnnotation[],
  file: string,
  func: string,
  getValue: (annotation: SourceAnnotation) => number,
): number {
  let maxLine = -1;
  let maxValue = 0;

  for (const annotation of annotations) {
    const value = getValue(annotation);

    if (
      value !== undefined &&
      annotation.file === file &&
      annotation.function === func
    ) {
      if (value > maxValue) {
        maxValue = value;
        maxLine = annotation.line;
      }
    }
  }

  return maxLine;
}

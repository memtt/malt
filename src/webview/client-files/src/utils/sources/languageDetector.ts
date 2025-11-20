/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/src/utils/sources/languageDetector.ts
*-----------------------------------------------------------
*    AUTHOR   : Emeric GUYON - 2025
***********************************************************/
/**
 * Language detection for syntax highlighting
 * Ported from Angular source-editor.js
 */

export type SupportedLanguage = 'C++' | 'Fortran' | 'Python';

/**
 * Detect programming language from file extension
 * @param filename - The filename with extension
 * @returns Language name for Prism.js
 */
export function detectLanguage(filename: string): SupportedLanguage {
  const ext = (/[.]/.exec(filename) ? /[^.]+$/.exec(filename) : undefined);
  const extension = ext ? (ext + '').toLowerCase() : '';

  switch (extension) {
    case 'f':
    case 'f90':
    case 'f77':
      return 'Fortran';
    case 'py':
      return 'Python';
    case 'c':
    case 'h':
    case 'cxx':
    case 'cpp':
    case 'hxx':
    case 'h++':
    case 'hpp':
    case 'ansic':
    default:
      return 'C++';
  }
}

/**
 * Get Prism.js CSS class for language
 * @param language - Language name
 * @returns CSS class for Prism.js
 */
export function getLanguageClass(language: SupportedLanguage): string {
  switch (language.toLowerCase()) {
    case 'fortran':
      return 'language-fortran';
    case 'python':
      return 'language-python';
    case 'c++':
    case 'c':
    default:
      return 'language-cpp';
  }
}

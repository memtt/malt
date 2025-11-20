/**
 * Prism.js initialization
 * Import all Prism components, plugins and themes in the correct order
 */

// Core Prism
import Prism from 'prismjs';

// Theme
import 'prismjs/themes/prism.css';

// Languages
import 'prismjs/components/prism-c';
import 'prismjs/components/prism-cpp';
import 'prismjs/components/prism-python';
import 'prismjs/components/prism-fortran';

// Plugins - CSS first, then JS
import 'prismjs/plugins/line-numbers/prism-line-numbers.css';
import 'prismjs/plugins/line-numbers/prism-line-numbers';

import 'prismjs/plugins/line-highlight/prism-line-highlight.css';
import 'prismjs/plugins/line-highlight/prism-line-highlight';

// Custom code annotator plugin
// Note: Custom theme styles are now in SourceCodeEditor.vue
import { initCodeAnnotator } from '@/lib/prism/codeAnnotator';

// Initialize custom plugins
initCodeAnnotator();

// Disable automatic highlighting
Prism.manual = true;

export default Prism;

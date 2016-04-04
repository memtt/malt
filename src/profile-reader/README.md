Profile reader
==============

This directory aimed at providing a faster profile (json) reader to manage big profile
files when reaching the limits of NodeJS (over ~200-400MB).

Here we used a fast C++ inplace json reader to stay aware of the memory usage for this kind of
corner cases.

Maybe we can port most of the `MaltProject.js` extraction functions into this tool and call it
from NodeJS to be faster and avoid playing with the limits of NodeJS. And it might permit to
use multi-threading to speeup the waling over the tree when needed and keep lower memory
consumption.

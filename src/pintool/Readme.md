Instrumentation with pintool
============================

MALT can also use binary instrumentation mode through pintool 
(http://software.intel.com/en-us/articles/pin-a-dynamic-binary-instrumentation-tool)

To compile :

make PIN_ROOT={PATH_TO_PINTOOL_ROOT_DIR}/

To use :

pin -t obj-intel64/mallocprof.so -- {YOUR_PROGRAM}

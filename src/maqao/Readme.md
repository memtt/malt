Instrumentation with pintool
============================

MALT can also use binary instrumentation mode through maqao.
(http://www.maqao.org/)

To instrument your code :

maqao instrument i=malt.mil {YOUR_PROGRAM}

To use :

malt ./{YOUR_PROGRAM}_mil.i

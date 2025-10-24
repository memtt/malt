Using -finstrument-functions
============================

MALT by default use a **backtrace** appproach to reconstruct the stacks. It has the
nice advantage to perform mostly everywhere but has the problem to be very slow.
This is the main source of the overhead of MALT when the app is making millions
of memory allocation.

Faster instrumentation
----------------------

In this case, in order to reduce the overhead you can recompile your application
(ideally also the libraries otherwise they will be not seen) by using :

.. code:: shell

    gcc -finstrument-functions -g ...

It will had some hooks at the entry/exit of each function so the stack is reconstruct
while diffing into the call graph and not for every malloc. When making lots of mallocs
it lower the overhead.

Then use malt by using :

.. code:: shell

    malt -s enter-exit ./my_program

Get the stack memory usage
--------------------------

This option is also required to reconstruct the memory used by the stack.

**Bug**: There is currently a limitation here. Due to the way GCC implement it, the
leaf function will not be measured in currentl implementation of MALT.

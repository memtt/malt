Other memory allocators
=======================

There is several very well done memory allocator implementations available in the
open-source community. As sometimes it can offer very nice performance or memory
consumption improvement in application, it is interesting to test them.

The one I know that behave very nicely depending on the patterns of the app are listed
here.

How to use a custom allocator
-----------------------------

If your application is built in a non static way, the easier way to try another
memory allocator is just to compile it and inject in in the allocation using :

.. code:: shell

    LD_PRELOAD=libjemalloc.so ./my_program
    LD_PRELOAD=libtcmalloc.so ./my_program
    #...

JeMalloc
--------

Implemented by John Evans, a very nice memory allocator, mostly optimal to reduce
the memory consumption of an applicaton. It has very nice algorithms to manage the
small blocks in an efficient way. It is also natively done for parallel codes.

When allocating lors of large segment, it tends to exchange them oftenly with the system
which for HPC application tend to increase the execution time.

I studied it a lot during my PhD. about memory managment and looks to me to have a greater
potential.

**URL**: https://jemalloc.net/

TCMalloc
--------

A memory allocator implemented by Google and very nicely done to take in account the cache
effects about memory management. It oftenly offer very nice performance but at the price
of the memory consumption depening on the allocation pattern of your code. It is also natively
done for parallel codes.

**URL**: https://github.com/gperftools/gperftools

Hoard
-----

The memory allocator written by Emery Berger, a well known researcher about memory management.
This one has nice featur about parallelism, and is historically a well inspirator about the ways
to handle parallelism at the allocator level.

**URL**: http://www.hoard.org/

Lockless allocator
------------------

Another memory allocator done for parallelism.

**URL**: http://locklessinc.com/downloads/

MPC Allocator
-------------

A memory allocator developped for the `MPC <https://mpc.hpcframework.com/>`_ (Multi-Processor Computing framework).
This is inspired about the work of the previous allocators and implemented by the core author of **MALT**.
It is secifically developped form NUMA architectures and permits to switch between a low memory profile (slower)
to a faster running mode (hight memory consumption). This one is specifically tuned to handle well the large
memory allocations done in HPC applications.

**URL**: https://github.com/cea-hpc/mpc-allocator

Mimalloc
--------

A memory allocator we tend to see in many posts about Python community done by Microsoft.

**URL**: https://github.com/microsoft/mimalloc

MATT : Memory Allocations Tracking Tools
========================================

What it is ?
------------

MATT is a memory tool to find where you allocate your memory. It also provide you some
statistics about memory usage and help to find memory leaks.

How to install
--------------

MATT use CMake for is build system you can install it by following the procudre :

	mkdir build
	cd build
	cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX={YOUR_PREFIX}
	make
	make test
	make install

How to use
----------

MATT currently provide a dynamic library you need to preload in your application to
wrap the default memory allocator. It provide two basic instrumentation modes.

By default MATT use backtrace to reconstruct you stack on malloc/free/... calls :

	LD_PRELOD=libmatt.so {YOUR_PROGRAM} [OPTIONS]

You can get better performance but less detailed stack by using option 
-finstrument-function or similar on your compiler. Then, you need to tel MATT to use
the "enter-exit" stack mode :

	MATT_STACK="enter-exit" LD_PRELOD=libmatt.so {YOUR_PROGRAM} [OPTIONS]

Config
------

You can provide a config file to MATT to setup some features. This file use the INI
format.

	MATT_CONFIG="myconfig.ini" LD_PRELOAD=libmatt.so {YOUR_PROGRAM} [OPTIONS]

Experimental pintool mode
-------------------------

MATT can also use binary instrumentation mode through pintool 
(http://software.intel.com/en-us/articles/pin-a-dynamic-binary-instrumentation-tool)

Please, check usage into src/pintool directory for this.

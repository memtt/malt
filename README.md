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

Build options
-------------

MATT build support several options to define with -D option of CMake :

 * -DENABLE_CODE_TIMING={yes|no} : Enable quick and dirty function to measure MATT internal
   performances.
 * -DENABLE_TEST={yes|no}        : Enable build of unit tests.
 * -DJUNIT_OUTPUT={yes|no}       : Enable generation of junit files for jenkins integration.
 * -DENABLE_VALGRIND={yes|no}    : Run unit tests inside valgrind memcheck and generate XML report.
 * -DPORTABILITY_OS={UNIX}       : Set portability build option to fix OS specific calls.
 * -DPORTABILITY_MUTEX={PTHREAD} : Set portability build option to select mutex implementation.

How to use
----------

MATT currently provide a dynamic library you need to preload in your application to
wrap the default memory allocator. It provide two basic instrumentation modes.

By default MATT use backtrace to reconstruct you stack on malloc/free/... calls :

	{YOUR_PREFIX}/bin/matt {YOUR_PROGRAM} [OPTIONS]

You can get better performance but less detailed stack by using option 
-finstrument-function or similar on your compiler. Then, you need to tel MATT to use
the "enter-exit" stack mode :

	MATT_STACK="enter-exit" LD_PRELOD=libmatt.so {YOUR_PROGRAM} [OPTIONS]

Config
------

You can provide a config file to MATT to setup some features. This file use the INI
format.

	MATT_CONFIG="myconfig.ini" LD_PRELOAD=libmatt.so {YOUR_PROGRAM} [OPTIONS]

Example of config file :

	[time]
	enabled=true          ; enable time profiles
	points=1000           ; keep 1000 points
	linar_index=false     ; use action ID instead of time

	[stack]
	enabled=true          ; enable stack profiles
	mode=backtrace        ; select stack tracing mode (backtrace|enter-exit)
	resolve=true          ; Automatically resolve symbol with addr2line at exit.

	[output]
	name=matt-%1-%2.%3    ; base name for output, %1 = exe, %2 = PID, %3 = extension
	indent=true           ; indent the output
	lua=true              ; enable LUA output
	json=true             ; enable json output
	callgrind=true        ; enable callgrind output
	config=true           ; dump current config

Using webview
-------------

You can use the webview by calling command `matt-webview` as :

	matt-webview [-p PORT] -i matt-YOUR_PROGRAM-1234.json

It will open a server listening localy on port 8080 so you can open your web browser
to connect to the web interface via http://localhost:8080.

If you are running the view remotly thought SSH you can redirect the ports by using :

	ssh -L 8080:localhost:8080 user@ssh-server

To use the webview you need to install the nodeJS package on your system : http://nodejs.org/.

Experimental pintool mode
-------------------------

MATT can also use binary instrumentation mode through pintool 
(http://software.intel.com/en-us/articles/pin-a-dynamic-binary-instrumentation-tool)

Please, check usage into src/pintool directory for this.

Generate code coverage of internal test suite :
-----------------------------------------------

To geneate test coverage, we need to setup CFLAGS for gcc+gcov :

        -DCMAKE_CXX_FLAGS="-O0 -fprofile-arcs -ftest-coverage"
        -DCMAKE_C_FLAGS="-O0 -fprofile-arcs -ftest-coverage"
        -DCMAKE_EXE_LINKER_FLAGS="-O0 -fprofile-arcs -ftest-coverage

You can do it simply by using :

        $ cmake .. -DTEST_COVERAGE=yes

or

        $ ../configure --enable-gcc-coverage

Now build, run the tests and call lcov and genhtml :

        $ make
        $ ./src/test/all_tests
        $ lcov -o out.info -c -d .
        $ genhtml -o html out.inf


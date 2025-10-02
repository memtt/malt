Project directories
===================

This document provides a short description of the MALT directory structure.

- /dev : Draft directory to put some development files (UML, scripts...)
- /cmake : Directory for all cmake scripts.
- /extern-deps : Directory for all embeded extern dependencies
- /src : Contain the main sources of the project.

About /extern-deps
------------------

The /extern-deps directory mainly contains sources as :

- /extern-deps/from-fftw : Extraction of cycle.h from FFTW as they provide it under MIT licence on their site.
/from-htopml : Extraction of some file from a personal project not available on net, but licensed under CeCILL-C.
It mostly contains the JSON outputter.
- I (Sébastien Valat) authorized reuse of those files under CeCILL-C or LGPL licenses.
- /extern-deps/gmock : Contain all gtest/gmock files as last version cannot be installed anymore and must be provided by projects or
found sources onto system (but too unsage).
- /extern-deps/iniparser : A simple INI file parser which does not provide clean installers.

About /src
----------

It is the main source directory containing all the important files of the project. It splits into sub-dirs for each component :

- /src/doc : Contain all the documentation files, mostly in markdown format for doxygen and gitlab/github.
- /src/integration : Provide some files to integrate MALT in other tools or in the system. Currently mostly integration files with MPI and wrapper scripts to help users.
- /src/lib : Main part of the MALT code with instrumentation library implementation (libmalt.so). It also provides  libmaltbase.so without malloc/calloc/... wrappers linked.
- /src/maqao : Provide the files for instrumentation with MAQAO.
- /src/pintool : Provide the files for instrumentation with Pintool.
- /src/trace-reader : Provide a small program to convert MALT traces into json format and to make some reduction/filtering.
- /src/webbiew : Provide the GUI as a web interface based on NodeJS + AnulgarJS + Bootstrap + D3JS + NVD3

About /src/lib
--------------

The lib sources are split in subdirectories, each contain a 'test' subdirectory with related unit tests based on gtest/gmock :

- /src/lib/common : Provide the common function used by the whole library (debug, locks....)
- /src/lib/portability : All platform dependent code must be placed here. You find more information about the conventions of this directory into XXXXX-TODO
- /src/lib/core : Contain the main classes used to run the MALT library.
- /src/lib/profiler : Contain the main entry point to use in wrappers, it provides the per thread and global profiler.
- /src/lib/tools : Provide some classes to wrap external tools or libraries (libelf, nm)
- /src/lib/valprof : Provide classes to profile values overs time.
- /src/lib/wrapper : Provide the wrapper to capture usage of standard functions (malloc/calloc/pthread_create...).
- /src/lib/test : Some complementary unit and integration tests.
- /src/lib/stacks : Internal stack representation.
- /src/lib/stack-tree : Stack trees used for internal storage.


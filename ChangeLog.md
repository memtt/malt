Change log
==========

1.2.4 - 03/10/2024
------------------

 - Fix issue in previous release making some unit tests broken (line numbers) by file header changing.
 - Fix cmake issue with libunwind find package under Gentoo with x86_64 + x86.
 - Add build summary in CMake script.

1.2.3 - 02/10/2024
------------------

 - Upgrade Express.JS used version due to security issue.
 - Remove javascript libs from the git repo, now download them and embed them at
   release time in distributed tarball.
 - Add authors to all source headers.
 - Fix some typo in documentation.
 - Webview now support big files.
 - Dev script to check portability over distributions via Podman.
 - Allow using malt webview forwarded via VSCode server.
 - Some fixes in ./configure
 - Use system google test if present.
 - Upgrade embedded version of google test.
 - Fix ways of finding MALT_ROOT due to readlink -f not supported on FreeBSD (now optional)
 - Make MALT install dir re-localisable.
 - Some tuning of address translation to support better large executables / libs.

1.2.2 - 28/06/2023
------------------

 - Add --disable-profiler on configure script to build only the webview.
 - Fix bugs reported by the new unit tests.
 - Fix build warnings on recent gcc & clang (debian 11).
 - Fix bug when using CC='' on configure script.
 - Upgrade the npm modules and fix usage of versions with security issues.
 - Improve unit testing of the profiler library.
 - Fix issue about binary files pointing relative source paths (issue #74).
 - Fix a bug in ELFReader generating a segfault when printing warning.
 - Keep track of the ASLR offset in the json file (to later post resolve the symbols)
 - Fix `(nil)` values in json file by using `0x0` instead for `void *` pointers.

1.2.1 - 17/10/2020
------------------

 - Build support for aarch64
 - Fix inlining issue with internal bactrace functions on recent GCC.

1.2.0 - 28/03/2020
------------------

 - Fix address solving due to ASLR & fPIE
 - Remove gomp malloc/free/realloc from call stacks
 - Portability over i386 and power9
 - Fix compiler warning
 - Extract current memory allocator name
 - Script to build deb & rpms with docker

1.1.0 - 30/08/2019
------------------

 - New configure script from NUMAPROF to be more generic and reusable.
 - Improve packaging fixing qt-webkit deps in RPM
 - Support dumping profile on demand via signal, to be enabled by user
 - Add option to dump every X second, implementation still ongoing
 - Webvew listen on demand on other than localhost
 - Improve fPIE + ASLR symbol resolution (Gentoo, Centos, Redhat)
 - Improve backtrace level cutting to better support clang LTO in O3
 - json format document up to date
 - Support dumping on selected IPC signal (SIGUSE....)
 - Fixes in rpm packaging
 - Fix quiet/silent mode and improve verbosity handling
 - Fix missing function on old NodeJS from redhat 7
 - Fix naming convention in .ini config file.
 - Fix issues on Xenial NodeJS

1.0.0 - 07/02/2018
------------------

 - Add scatter plots (lifetime,size) and (size,time).
 - Add maltEnable/maltDisable to limit to sub-part of a program.
 - Profile file is not anymore indented by default
 - Add options to reduce size of output file (stackTree and loopSuppress). Not enabled by default.
 - Add DebugDummy to avoid unneeded allocation in Release mode
 - Add help with MPI in README
 - Add manpages for malt, malt-passwd and malt-webserver
 - Add packaging files for gentoo & archlinux
 - Use a more compress stack tree in json file
 - Add filter to only instrument the given executable and disabling childs
 - Add warnings about global varibales if the app use too many of them
 - Add QT5 webkit webview wrapper.

0.3.0 - 16/06/2015
------------------

 - Add http authentication to secure the web GUI.
 - Add silent option (-q/--quiet)
 - Add password authentication inside http protocol to secure the web GUI
 - Add support for packaging (Fedora/Centos/Debian/Ubuntu...)
 - Fix error in unit tests
 - Fix bug in ELFReader

0.2.0 - 29/04/2015
------------------

 - Add scatter chart on ralloc page.
 - Get grunt script to generate shorter standalone version with compressed JS/CSS
 - Add support for data export and svg export on some charts
 - Bugfix: add missing modules in package.js
 - Bugfix: index times on time charts

0.1.0 - 26/03/2015
------------------

 - First tagged release don't list all new things.

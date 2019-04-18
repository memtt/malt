MALT : Malloc Tracker
=====================

[![Build Status](https://travis-ci.org/memtt/malt.svg?branch=master)](https://travis-ci.org/memtt/malt)

What is it
----------

MALT is a memory tool to find where you allocate your memory. It also provides you some
statistics about memory usage and help to find memory leaks.

![MALT GUI](https://memtt.github.io/malt/images/screenshots/screenshot-12.png)

Dependencies
------------

MALT depends on the presence of :

- binutils (nm and add2line) to extract symbols. Tested version is 2.24.

It optionally depends on :

- nodejs (<http://nodejs.org/>) to run the webview GUI. Tested version is 0.10.30.
- libelf (<http://www.mr511.de/software/english.html>) to extract global variable list from executables and libs. Tested version is 0.128.
- libunwind (<http://www.nongnu.org/libunwind/>) as an alternative implementation of glibc backtrace method. Tested version is 1.1.

How to install
--------------

MALT use CMake for the build system but provide a simple configure wrapper for users
familiar with autotools packaging so you can install by following the procedure :

```shell
mkdir build
cd build
../configure --prefix={YOUR_PREFIX}
make
make test
make install
```

If you want more advance usage, you need to call cmake by yourself so you can install it
by following the procedure :

```shell
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX={YOUR_PREFIX}
make
make test
make install
```

Build options
-------------

MALT build support several options to define with -D option of CMake :

- `-DENABLE_CODE_TIMING={yes|no}` : Enable quick and dirty function to measure MALT internal
  performances.
- `-DENABLE_TEST={yes|no}`        : Enable build of unit tests.
- `-DJUNIT_OUTPUT={yes|no}`       : Enable generation of junit files for jenkins integration.
- `-DENABLE_VALGRIND={yes|no}`    : Run unit tests inside valgrind memcheck and generate XML report.
- `-DPORTABILITY_OS={UNIX}`       : Set portability build options to fix OS specific calls.
- `-DPORTABILITY_MUTEX={PTHREAD}` : Set portability build option to select mutex implementation.

How to use
----------

MALT currently provides a dynamic library you need to preload in your application to
wrap the default memory allocator. It provides two basic instrumentation modes.

By default MALT use backtrace to reconstruct you stack on malloc/free/... calls :

```shell
{YOUR_PREFIX}/bin/malt {YOUR_PROGRAM} [OPTIONS]
```

You can get better performance but less detailed stack by using option
-finstrument-function or similar for your compiler. Then, you need to tel MALT to use
the "enter-exit" stack mode :

```shell
{YOUR_PREFIX}/bin/malt -s=enter-exit {YOUR_PROGRAM} [OPTIONS]
```

The malt script only provides a wrapper to automatically preload a dynamic library
into the executable, you can also do it by hand in cas of issues :

```shell
LD_PRELOAD={YOUR_PREFIX}/lib/libmalt.so {YOUR_PROGRAM} [OPTIONS]
```

Options to compile your program
-------------------------------

MALT work out of the box with your program but it required you to compile your program with
debug options (`-g`) to get access to the source code attached to each call sites.

It might also be better to use `-O0` or use `-fno-inline` to disable inlining which might
provide more accurate call stacks to you.

How to use with MPI
-------------------

MALT also provides a lightweight support of MPI to generate profile files named with MPI rank ID instead of process ID.
In order to support this you first need to compile the MPI interface on top of your MPI. It will generate a
small library in your home directory.

```shell
{YOUR_PREFIX}/bin/malt --prep-mpi [mpicxx]
```

Caution it will link malt to the current MPI version you are using, if you want to switch to another you will need to
redo the previous command.

Then to profile you mpi application proceed like :

```shell
mpirun -np X {YOUR_PREFIX}/bin/malt --mpi {YOUR_PROGRAM} [OPTIONS]
```

Using webview
-------------

You can use the webview by calling command `malt-webview` as :

```shell
malt-webview [-p PORT] [--no-auth] -i malt-YOUR_PROGRAM-1234.json
```

It will open a server listening locally on port 8080 so you can open your web browser
to connect to the web interface via <http://localhost:8080>.

At first usage malt-webview will create the password file `$HOME/.malt/passwd` and ask you a
protection password for http authentification. You can change it at any time with

```shell
malt-passwd {USER}
```

If you are running the view remotely thought SSH you can redirect the ports by using :

```shell
ssh -L 8080:localhost:8080 user@ssh-server
```

To use the webview you need to install the nodeJS package on your system : <http://nodejs.org/>.

Config
------

You can provide a config file to MALT to setup some features. This file uses the INI
format. With the malt script :

```shell
{YOUR_PREFIX}/bin/malt -c=config.ini" {YOUR_PROGRAM} [OPTIONS]
```

By hand :

```shell
MALT_CONFIG="config.ini" LD_PRELOAD=libmalt.so {YOUR_PROGRAM} [OPTIONS]
```

Example of config file :

```ini
[time]
enabled=true          ; enable time profiles
points=1000           ; keep 1000 points
linar-index=false     ; use action ID instead of time

[stack]
enabled=true          ; enable stack profiles
mode=backtrace        ; select stack tracing mode (backtrace|enter-exit)
resolve=true          ; Automatically resolve symbols with addr2line at exit.
libunwind=false       ; Enable of disable usage of libunwind to backtrace.
skip=4                ; Number of stack frame to skip in order to cut at malloc level

[output]
name=malt-%1-%2.%3    ; base name for output, %1 = exe, %2 = PID, %3 = extension
lua=true              ; enable LUA output
json=true             ; enable json output
callgrind=true        ; enable callgrind output
indent=false          ; indent the output profile files
config=true           ; dump current config
verbosity=default     ; malt verbosity level (silent, default, verbose)
stack-tree=false       ; store the call tree as a tree (smaller file, but need conversion)
loop-suppress=false    ; Simplify recursive loop calls to get smaller profile file if too big

[max-stack]
enabled=true          ; enable of disable strack size tracking (require -finstrument-functions)

[distr]
alloc-size=true       ; generate distribution of allocation size
realloc-jump=true     ; generate distribution of realloc jumps

[trace]
enable=false          ; enable dumping allocation event tracing (not yet used by GUI)

[info]
hidden=false          ; try to hide possible sensible names from profile (exe, hostname...)

[filter]
exe=                  ; Only apply malt on given exe (empty for all)
childs=true           ; Instrument child processes or not
enabled=true          ; Enable or disable MALT when threads start

[dump]
on-signal=             ; Dump on signal. Can be comma separated list from SIGINT, SIGUSR1,
                       ; SIGUSR2... help, avail (limited to only one dump)
after-seconds=0        ; Dump after X seconds (limited to only one time)
```

Option values can be overridden on the fly with command :

```shell
{YOUR_PREFIX}/bin/malt -o "stack:enabled=true;output:indent=true;" {YOUR_PROGRAM} [OPTIONS]
```

Environnement variables
-----------------------

If you do not use the malt wrapper and use directly LD_PRELOAD you can use the Environnement variables :

```shell
MALT_OPTIONS="stack:enabled=true;output:indent=true;"
MALT_CONFIG="config.ini"
MALT_STACK="libunwind"
```

Analysing sub-parts
-------------------

If you run on a really big program doing millions of allocation you might get a big overhead, and maybe
you are just interested in a sub-part of the program. You can do it by including `malt/malt.h` in
your files and use `maltEnable()` an `maltDisable()` to controle MALT on each thread. It is also a nice
way to detect leaks of sub-parts of your code.

```c
#include <malt/controler.h>

int main()
{
    maltDisable();
    //ignored
    malloc(16);

    maltEnable();
    //tracked
    malloc(16);
}
```

You will need to link the `libmalt-controler.so` to get the default fake symbols when not using MALT.
You can also just provide the two empty functions in your own dynamic library (not static).

If you have some allocation not under your control before your first call you can disable MALT by default
on threads using the `filter:enabled` option, then enable it by hand.

About stacks
------------

MALT use two ways to rebuild stacks, the default one relies on glibc backtrace but we observe several
segfaults on some intel tools such as Intel OpenMP and Intel MPI so we also provide a more robust
approach based on libunwind if present on your system at build time. You can provide it with :

```shell
../configure --with-libunwind=PREFIX
```

or on cmake :

```shell
cmake -DLIBUNWIND_PREFIX=PREFIX ..
```

You now can use it with malt by using :

```shell
malt -s libunwind {PROGRAM}
```

The alternative relies on function instrumentation by adding prove on start/end for each function.
It can be done by using -finstrument-function on your compiler just as described in "How to use" section
or by using binary instrumentation tools just as explained at the end of this document.

If you want to use the source instrumentation appraoch, you need to recompiler your program
and the interesting libraries with :

```sh
gcc -finstrument-functions
```

Then running malt with :

```sh
${YOUR_PREFIX}/bin/malt -s enter-exit {YOUR_PROGRAM}
```

Tracking stack size
-------------------

Malt can also track the memory used by stacks over time, but for this support it is required to
enable a compiler flag :

```shell
gcc -finstrument-functions {YOUR FILES}
```

Experimental pintool mode
-------------------------

MALT can also use binary instrumentation mode through pintool
(<http://software.intel.com/en-us/articles/pin-a-dynamic-binary-instrumentation-tool>)

Please, check usage into src/pintool directory.

Experimental maqao mode
-----------------------

MALT can also use binary instrumentation with MAQAO (<http://maqao.org/>).

Please check usage into src/maqao directory.

Dealing with big files
----------------------

In some cases you might get really big files. I get up to 600 MB on one code. The issue is that you
cannot load this kind of file into nodejs due to some limits into the string used to read the file
into json parsor functions.

The first alternative is to try to generate more compressed file by enabling usage of `stackTree` output
options to store the stacks as a tree into the file. It is more efficient in terms of space (in the 600 MB
case it lower the file to 200 MB) but need an on-fly conversion by the server to get back the supported format.

```shell
malt -o "output:stackTree=true" ./PROGRAM
```

Currently you can still find cases where you cannot load the file into nodejs, I'm working on a workaround.
Please provide me your files if it appends. By compressing it in gzip you will get less than 30-40 MB.

Packaging
---------

You can find packaging instructions inside packaging/README.md.
For quicker use you can use the dev/packagin.sh script which do
the steps automatically.

Installation in non-standard directory
--------------------------------------

If you install MALT in a directory other than `/usr` and `/usr/local`, eg. in your home, you might
be interested by setting some environment variables integrating it to your shell :

```shell
export PATH=${PREFIX}/bin:$PATH
export MANPATH=${PREFIX}/share/man:$MANPATH
```

`LD_LIBRARY_PATH` is not required as the `malt` command will use the full path to get access the
internal `.so` file.

Similar tools
-------------

If you search similar tools all over the web you might find:

- Heaptrack: A Heap Memory Profiler for Linux: KDE/heaptrack: <https://github.com/KDE/heaptrack>
- Memoro: A detailed Heap Profiler : <https://epfl-vlsc.github.io/memoro/>
- Memtrail: <https://github.com/jrfonseca/memtrail>
- MTuner: <https://milostosic.github.io/MTuner/>
- Profiler provided with google allocator: [Google Heap Profiler](https://code.google.com/p/gperftools/wiki/GooglePerformanceTools)
- [Valgrind memcheck](http://valgrind.org/)
- Valgrind massif: [Valgrind massif](http://valgrind.org/) with [Massif visualizer](https://projects.kde.org/projects/extragear/sdk/massif-visualizer)
- [Dr. Memory](http://www.drmemory.org/)
- Commercial tool, [Parasoft Insure++](https://www.parasoft.com/product/insure/)
- Commercial tool, [Unicom PurifyPlus](http://unicomsi.com/products/purifyplus/) (previously IBM)
- [Tau](https://www.cs.uoregon.edu/research/tau/home.php) is more a communication profiling tool for HPC apps, but it offers a [memory module](https://www.cs.uoregon.edu/research/tau/docs/old/bk05ch04.html)
- Similar approach than MALT for the backend: [IgProf](http://igprof.org/)
- A debug malloc library: [Dmalloc](http://dmalloc.com/)
- Profiling and leak detection: [MemProf](http://people.redhat.com/otaylor/memprof/)
- [Malloc count](http://panthema.net/2013/malloc_count/)
- [mpatrol](http://mpatrol.sourceforge.net/)
- Tracing tool for parallel programs: [EZTrace](http://eztrace.gforge.inria.fr/)
- Find Obsolete Memory: [FOM Tools](https://gitlab.cern.ch/fom/FOM-tools/wikis/home)

If ever I missed new ones, you can also look on the repos of this person keeping an up-to-date list:
<https://github.com/MattPD/cpplinks/blob/master/performance.tools.md>

Parallel allocators
-------------------

If you search some parallel memory allocators, you can find those one on the net:

- [Jemalloc (facebook, firefox)](http://www.canonware.com/jemalloc/)
- [TCMalloc (google)](https://github.com/gperftools/gperftools)
- [Hoard](http://www.hoard.org/)
- [Lockless allocator](http://locklessinc.com/downloads/)
- [MPC](http://mpc.hpcframework.paratools.com/) memory allocator (look into mpcframework/MPC_Allocator)

License
-------

MALT is distributed under CeCILL-C license (LGPL compatible).

Discussion
----------

You can join the google group to exchange ideas and ask questions : <https://groups.google.com/forum/#!forum/memtt-malt>.

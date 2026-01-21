Options
=======

MALT supports several options which can be passed by **file** or **command line**.

The options are represented into a **two level** tree with **groups** and **options**
to follow the **ini** file standard.

Command line
------------

The **command line** options follow the simple semantic :

.. code-block:: shell

    # pass the options at launch time
    malt -o {GROUPE}:{OPTION}={VALUE} -o {GROUPE2}:{OPTION2}={VALUE2} ./my_program

    # In a single option
    malt -o "{GROUPE}:{OPTION}={VALUE};{GROUPE2}:{OPTION2}={VALUE2}" ./my_program

Configuration file
------------------

You can provide a **config file** to MALT to setup some features. This file uses the **INI**
format. With the malt script :

.. code-block:: shell

    malt -c config.ini {YOUR_PROGRAM} [OPTIONS]

Note you can use **file** plus override some options by adding `-o/--option` afterward :

.. code-block:: shell

    malt -c config.ini -o time:points=5000 {YOUR_PROGRAM} [OPTIONS]

Available options
-----------------

Example of config file :

.. code-block:: ini

    [time]
    enabled=true           ; enable time profiles
    points=512             ; keep 512 points
    linar-index=false      ; use action ID instead of time

    [stack]
    enabled=true           ; enable stack profiles
    mode=backtrace         ; select stack tracing mode (backtrace|enter-exit)
    resolve=true           ; Automatically resolve symbols with addr2line at exit.
    libunwind=false        ; Enable of disable usage of libunwind to backtrace.
    skip=4                 ; Number of stack frame to skip in order to cut at malloc level

    [sampling]
    enabled=false          ; Sample and instrument only some stack.
    bandwidth=4093         ; Instrument the stack when seen passed 4K-3 bytes of alloc requests (ideally should be prime number).
    count=571              ; Instrument the stack when seen passed 4K-3 bytes of alloc requests (ideally should be prime number).

    [addr2line]
    bucket=350             ; Handle the addr2line calls by buckets and treat each bucket in parallel.
    threads=8              ; Number of threasd to use to call addr2line in parallel.
    huge=52428800          ; For larger elf files, do not treat them in parallel nor buckets.

    [output]
    name=malt-%1-%2.%3     ; base name for output, %1 = exe, %2 = PID, %3 = extension
    lua=true               ; enable LUA output
    json=true              ; enable json output
    callgrind=true         ; enable callgrind output
    indent=false           ; indent the output profile files
    config=true            ; dump current config
    verbosity=default      ; malt verbosity level (silent, default, verbose)
    stack-tree=false       ; store the call tree as a tree (smaller file, but need conversion)
    loop-suppress=false    ; Simplify recursive loop calls to get smaller profile file if too big
    verbosity=default      ; Malt verbosity level (silent, default, verbose).
	stack-tree=false       ; Store the call tree as a tree (smaller file, but need conversion in the reader);

    [max-stack]
    enabled=true           ; enable of disable strack size tracking (require -finstrument-functions)

    [distr]
    alloc-size=true        ; generate distribution of allocation size
    realloc-jump=true      ; generate distribution of realloc jumps

    [trace]
    enable=false           ; enable dumping allocation event tracing (not yet used by GUI)

    [info]
    hidden=false           ; try to hide possible sensible names from profile (exe, hostname...)

    [filter]
    exe=                   ; Only apply malt on given exe (empty for all)
    childs=true            ; Instrument child processes or not
    enabled=true           ; Enable or disable MALT when threads start
    ranks=                 ; Instrument only the given ranks from list as : 1,2-4,6

    [dump]
    on-signal=             ; Dump on signal. Can be comma separated list from SIGINT, SIGUSR1,
                           ; SIGUSR2... help, avail (limited to only one dump)
    after-seconds=0        ; Dump after X seconds (limited to only one time)
    on-sys-full-at=        ; Dump when system memory become full at x%, xG, xM, xK, x  (empty to disable).
    on-app-using-rss=      ; Dump when RSS of the app reach the given limit in %, G, M, K (empty to disable).
    on-app-using-virt=     ; Dump when Virtual Memory of the app reach limit in %, G, M, K (empty to disable).
    on-app-using-req=      ; Dump when Requested Memory of the app reach limit in %, G, M, K (empty to disable).
    on-thread-stack-using= ; Dump when one stack reach limit in %, G, M, K (empty to disable).
    on-alloc-count=        ; Dump when number of allocations reach limit in G, M, K (empty to disable).
    watch-dog=false        ; Run an active thread spying continuouly the memory of the app, not only sometimes.

    [python]
    enabled=true           ; Enable of disable python instrumentation.
    stack=enter-exit       ; Select the Python stack instrumentation mode (backtrace, enter-exit, none).
    mix=false              ; Mix C stack with the python ones to get a uniq tree instread of two distincts
                           ; (not this adds overhead).
    obj=true               ; Instrument of not the OBJECT allocator domain of python.
    mem=true               ; Instrument of not the MEM allocator domain of python.
    raw=true               ; Instrument of not the RAW allocator domain of python.
    hide-imports=true      ; Hide the stack of memory allocations done by imports.
	mode=profile           ; Define the python instrumentation mode which define how to get the line number ('profile' or 'trace').

    [c]
    malloc=true            ; Track C malloc.
    mmap=true              ; Track C mmap direct calls.

    [tools]
    nm=true                ; Enable usage of NM to find the source locatoin of the global variables.
    nm-max-size=50M        ; Do not call nm on .so larger than 50 MB to limit the profile dump overhead.

Section `time`
--------------

This set of option permits to configure the time charts about the dynamic of the application.

Option `time:enabled`
^^^^^^^^^^^^^^^^^^^^^

Enable support of tracking state values over time to build time charts.

**Default**: true.

.. code-block:: shell

    malt -o time:enabled=true ./my_program
    malt -o time:enabled=false ./my_program

Option `time:points`
^^^^^^^^^^^^^^^^^^^^

Define the number of points used to discretized the execution time of the application.

**Default**: 512.

.. code-block:: shell

    malt -o time:points=512 ./my_program

Option `time:linear`
^^^^^^^^^^^^^^^^^^^^

Do not use time to index data but a linear value increased on each call (might be interesting
not to shrink intensive allocation steps on a long program which mostly not do allocation over the run.

**Default**: false.

.. code-block:: shell

    malt -o time:linear=true ./my_program
    malt -o time:linear=false ./my_program

Section `stack`
---------------

Option `stack:enabled`
^^^^^^^^^^^^^^^^^^^^^^

Enable support of stack tracking.

**Default**: true.

.. code-block:: shell

    malt -o stack:enabled=true ./my_program
    malt -o stack:enabled=false ./my_program

Option `stack:mode`
^^^^^^^^^^^^^^^^^^^

Override by `-s` option from the command line, it set the stack tracking method to use. See `-s` documentation
for more details. The available values are `enter-exit`, `backtrace`, `python`, `none`. By default backtrace
is used as it works out of the box everywhere. It is slower than enter-exit but this last one needs recompilation
of the code with `-finstrument-functions` and see only the libs recompiled with this option. `python` permit
to project all the C allocation directly into the python stack domain so it hides the C underwood. Good to
improve performance over python when you are not interested into the C details.

**Default**: backtrace.

.. code-block:: shell

    malt -o stack:mode=backtrace ./my_program
    malt -o stack:mode=enter-exit ./my_program
    malt -o stack:mode=python ./my_program
    malt -o stack:mode=none ./my_program

Option `stack:resolve`
^^^^^^^^^^^^^^^^^^^^^^

Enable symbol resolution at the end of execution to extract full names and source location if debug options
is available.

**Default**: true.

.. code-block:: shell

    malt -o stack:resolve=true ./my_program
    malt -o stack:resolve=false ./my_program

Option `stack:libunwind`
^^^^^^^^^^^^^^^^^^^^^^^^

Use linunwind backtrace method instead of the one from glibc.

**Default**: false.

.. code-block:: shell

    malt -o stack:libunwind=true ./my_program
    malt -o stack:libunwind=false ./my_program

Option `stack:skip`
^^^^^^^^^^^^^^^^^^^

In backtrace mode, the backtrace is called inside our instrumentation function itself called by malloc/free....
In order to skip our internal code we need to remove them. But on some compilers or distros, inlining and LTO
configuration make wrong detection. This can be fixed by playing with this value to keep malloc/free/calloc
as leaf for every calltree extracted by MALT.

If you don't see malloc as leaf, you should increase this value. Decrease if you start to see MALT internal
function inside.

Notice in some cases (Gentoo, Gcc-8) we start to see LTO trashing totaly the malloc/free call in O3 mode 
normaly seen via backtrace. There is currently no other solution except recompiling in O0/O1 to avoid
or maybe disable LTO optimizations or consider not having the exact location of the malloc itself.

**Default**: 4.

.. code-block:: shell

    malt -o stack:skip=4 ./my_program

Option `sampling:enabled`
^^^^^^^^^^^^^^^^^^^^^^^^^

Enable sampling mode for the stacks by captuing only for a few mallocs. It is less accurate than a full
profile but cost less in memory and CPU. In sampling mode, the stack is processed only when we seen passed
a few bytes allocated, otherwise we consider the last seen call stack. With python you should also
enable the backtrace mode for solving stacks via `python:stack=backtrace`.

It will use the options `sampling:volume` and `sampling:count` to know at which rate to sample.

**Default**: false.

.. code-block:: shell

    malt -o sampling:enabled=true ./my_program
    malt -o sampling:enabled=false ./my_program

Option `sampling:volume`
^^^^^^^^^^^^^^^^^^^^^^^^

Define the amount of data seen passed between two samples. Idealy this should be a prime number to avoid
some multiple base biases..

It is completed by also sampling on count with `sampling:count`.

**Default**: 4093.

.. code-block:: shell

    malt -o sampling:volume=4093 ./my_program
    malt -o sampling:volume=5242883 ./my_program
    malt -o sampling:volume=10485767 ./my_program
    malt -o sampling:volume=20971529 ./my_program

Option `sampling:count`
^^^^^^^^^^^^^^^^^^^^^^^

Define the number of operations passing between two sampling. Idealy this should be a prime number to avoid
some multiple base biases..

It is completed by also sampling on count with `sampling:volume`.

**Default**: 571.

.. code-block:: shell

    malt -o sampling:count=13 ./my_program
    malt -o sampling:count=31 ./my_program
    malt -o sampling:count=67 ./my_program
    malt -o sampling:count=67 ./my_program
    malt -o sampling:count=257 ./my_program
    malt -o sampling:count=571 ./my_program

Option `addr2line:bucket`
^^^^^^^^^^^^^^^^^^^^^^^^^

Define the number of addresses to place in the bucket to resolve while calling `addr2line`.
Each bucket is executed in parallel so it goes faster. Except for the **huge** ELF files which
are treated as a single bucket and in sequentiel, not to kill the memory in MPI and not
loading the large file many times.

**Default**: 350.

.. code-block:: shell

    malt -o addr2line:bucket=350 ./my_program

Option `addr2line:threads`
^^^^^^^^^^^^^^^^^^^^^^^^^^

Define the number of threads to use to make multiple calls to `addr2line` in parallel.

**Default**: 8.

.. code-block:: shell

    malt -o addr2line:threads=8 ./my_program

Option `addr2line:huge`
^^^^^^^^^^^^^^^^^^^^^^^

Define the threashold for bucket splitting. For files larger than this limit, consider them
as huge and not using nor buckets, not parallelism not to trash the memory of the node with
too many large memory usage processes (especially in MPI).

**Default**: 52428800 (50 MB).

.. code-block:: shell

    malt -o addr2line:huge=52428800 ./my_program

Section `output`
----------------

Option `output:name`
^^^^^^^^^^^^^^^^^^^^

Define the name of the profile file. %1 is replaced by the program name, %2 by the PID or MPI rank and %3 by extension.

**Default**: malt-%1-%2.%3.

.. code-block:: shell

    malt -o output:name=malt-%1-%2.%3 ./my_program

Option `output:lua`
^^^^^^^^^^^^^^^^^^^

Enable output in LUA format (same structure as JSON files but in LUA).

**Default**: false

.. code-block:: shell

    malt -o output:lua=false ./my_program
    malt -o output:lua=true ./my_program

Option `output:json`
^^^^^^^^^^^^^^^^^^^^

Enable output of the default JSON file format.

**Default**: true

.. code-block:: shell

    malt -o output:json=true ./my_program
    malt -o output:json=false ./my_program

Option `output:callgrind`
^^^^^^^^^^^^^^^^^^^^^^^^^

Enable output of the compatibility format with callgrind/kcachegrind. Cannot contain all data but can be used
with compatible existing tools.

**Default**: false

.. code-block:: shell

    malt -o output:callgrind=false ./my_program
    malt -o output:callgrind=true ./my_program

Option `output:indent`
^^^^^^^^^^^^^^^^^^^^^^

Enable indentations in the JSON/LUA files. Useful for debugging but generate bigger files.
This feature is more for debugging and developpement process.

**Default**: false

.. code-block:: shell

    malt -o output:indent=false ./my_program
    malt -o output:indent=true ./my_program

Option `output:config`
^^^^^^^^^^^^^^^^^^^^^^

Dump the config INI file.

**Default**: false

.. code-block:: shell

    malt -o output:config=false ./my_program
    malt -o output:config=true ./my_program

Option `verbosity`
^^^^^^^^^^^^^^^^^^

Set the verbosity mode of MALT. By `default` it print at start and end. You can use `silent` mode to disable any ouptput for 
example if you instrument shell script parsing the output of child processes. You can also use `verbose` to have more 
debugging infos in case if does not work as expected, mostly at the symbol extraction step while dumping outputs.

**Default**: default

.. code-block:: shell

    malt -o output:verbosity=default ./my_program
    malt -o output:verbosity=silent ./my_program
    malt -o output:verbosity=verbose ./my_program

Option `stack-tree`
^^^^^^^^^^^^^^^^^^^

Enable storage of the stacks as a tree inside the output file. It produces smaller files but require conversion
at storage time and loading time to stay compatible with the basic expected format. You can use this option
to get smaller files. In one case it lowers a 600 MB file to 200 MB to give an idea.

**Note**: This is not yet supported by the new webview in written in C++ since **1.4.0**.

**Default**: false

.. code-block:: shell

    malt -o output:stack-tree=false ./my_program
    malt -o output:stack-tree=true ./my_program

Option `output:loop-suppress`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Enable recursive loop calls to remove them and provide a more simplified equivalent call stack. It helps
to reduce the size of profiles from applications using intensively this kind of call chain. In one case it lowers
file from 200 MB to 85 MB. It can help if nodejs failed to load the fail because of the size. This parameter
can also provide more readable stacks as you don't care to much how many times you cycle to call loops you
just want to see one of them.

**Default**: false

.. code-block:: shell

    malt -o output:loop-suppress=false ./my_program
    malt -o output:loop-suppress=true ./my_program

Section `max-stack`
-------------------

Option `max-stack:enabled`
^^^^^^^^^^^^^^^^^^^^^^^^^^

Enable or disable the tracking of stack size and memory used by functions on stacks 
(require  `--finstrument-function` on your code to provide data).

**Default**: true

.. code-block:: shell

    malt -o max-stack:enabled=true ./my_program
    malt -o max-stack:enabled=false ./my_program

Section `distr`
---------------

Option `distr:alloc-size`
^^^^^^^^^^^^^^^^^^^^^^^^^

Generate distribution of the allocated chunk size.

**Default**: true

.. code-block:: shell

    malt -o distr:alloc-size=true ./my_program
    malt -o distr:alloc-size=false ./my_program

Option `distr:realloc-jump`
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Generate distribution of the realloc size jumps.

**Default**: true

.. code-block:: shell

    malt -o distr:realloc-jump=true ./my_program
    malt -o distr:realloc-jump=false ./my_program

Section `trace`
---------------

Option `trace:enabled`
^^^^^^^^^^^^^^^^^^^^^^

Enable or disable the tracing (currently not used by the GUI, work in progress).

**Default**: false

.. code-block:: shell

    malt -o trace:enabled=false ./my_program
    malt -o trace:enabled=true ./my_program

Section `info`
--------------

Option `info:enabled`
^^^^^^^^^^^^^^^^^^^^^

Enable hiding execution information. This option remove some possibility sensitive information
from the output file, like executable names, hostname and command options. It is still recommended
taking a look at the file for example to replace the paths which might also be removed.
This option target some companies which might want to hide their internal applications when exchanging
with external partners.

**Default**: false

.. code-block:: shell

    malt -o info:enabled=false ./my_program
    malt -o info:enabled=true ./my_program

Section `filter`
----------------

Option `filter:exe`
^^^^^^^^^^^^^^^^^^^

Enable filtering of executable to enable MALT and ignore otherwise. By default empty value enable
MALT on all executable.

**Default**: empty

.. code-block:: shell

    malt -o filter:exe= ./my_program
    malt -o filter:exe=my_program ./my_program

Option `filter:childs`
^^^^^^^^^^^^^^^^^^^^^^

Enable instrumentation of children processes or not. By default instruments all.

**Default**: true

.. code-block:: shell

    malt -o filter:childs=true ./my_program
    malt -o filter:childs=false ./my_program

Option `filter:enabled`
^^^^^^^^^^^^^^^^^^^^^^^

Enable profiling by default. Can be disable to be able to activate via C function call in the app 
when you want.

**Default**: true

.. code-block:: shell

    malt -o filter:enabled=true ./my_program
    malt -o filter:enabled=false ./my_program

Option `filter:ranks`
^^^^^^^^^^^^^^^^^^^^^

When running in MPI mode, instrument only the given ranks. The list is provided under the form
`1,2-4,6`.

**Default**: empty

.. code-block:: shell

    malt -o filter:ranks= ./my_program
    malt -o filter:ranks=10 ./my_program
    malt -o filter:ranks=1-8 ./my_program
    malt -o filter:ranks=1-8,20,21-27 ./my_program

Section `dump`
--------------

Option `dump:on-signal`
^^^^^^^^^^^^^^^^^^^^^^^

Will dump on given signal. Can be on or comma separated list of: `SIGINT`, `SIGUSR1`, `SIGUSR2`, ...
Notice profiling will currently stop from this point app will continue without profiling.
To be fixed latter.
You can get the list of availble list by using `help` or `avail` in place of name.

**Default**: empty

.. code-block:: shell

    malt -o dump:on-signal= ./my_program
    malt -o dump:on-signal=SUGUSR1 ./my_program
    malt -o dump:on-signal=SIGINT,SIGUSR1,SIGUSR2 ./my_program

Option `dump:after-seconds`
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Will dump profile after X seconds.
Notice profiling will currently stop from this point app will continue without profiling.
To be fixed latter.

**Default**: empty

.. code-block:: shell

    malt -o dump:after-seconds= ./my_program
    malt -o dump:after-seconds=60 ./my_program

Option `dump:on-sys-full-at`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Will dump if the system memory if globaly filled at x%. Use empty string to disable.
Remark it consider free the free memory and the cached memory. A good value in practice is
more 70% / 80% than going to 95% due to necessity to let room for the cached memory and because
it will starts to swap before that. Consider also that MALT itself adds up memory on top of your
one (considered in the % here.). Values can be in %, K, M, G by ending with the corresponding
character.

**Default**: empty

.. code-block:: shell

    malt -o dump:on-sys-full-at= ./my_program
    malt -o dump:on-sys-full-at=80% ./my_program
    malt -o dump:on-sys-full-at=4096K ./my_program
    malt -o dump:on-sys-full-at=100M ./my_program
    malt -o dump:on-sys-full-at=10G ./my_program

Option `dump:on-app-using-rss`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Will dump if the application reach the given RSS limit. The value is given in % of the global memory
available or in K, M, G. Empty to disable (default).

**Default**: empty

.. code-block:: shell

    malt -o dump:on-app-using-rss= ./my_program
    malt -o dump:on-app-using-rss=80% ./my_program
    malt -o dump:on-app-using-rss=4096K ./my_program
    malt -o dump:on-app-using-rss=100M ./my_program
    malt -o dump:on-app-using-rss=10G ./my_program

Option `dump:on-app-using-virt`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Will dump if the application reach the given virtual memory limit. The value is given in % of the global memory
available or in K, M, G. Empty to disable (default).

**Default**: empty

.. code-block:: shell

    malt -o dump:on-app-using-virt= ./my_program
    malt -o dump:on-app-using-virt=80% ./my_program
    malt -o dump:on-app-using-virts=4096K ./my_program
    malt -o dump:on-app-using-virt=100M ./my_program
    malt -o dump:on-app-using-virt=10G ./my_program

Option `dump:on-app-using-req`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Will dump if the application reach the given requested memory limit. The value is given in % of the global memory
available or in K, M, G. Empty to disable (default).

**Default**: empty

.. code-block:: shell

    malt -o dump:on-app-using-req= ./my_program
    malt -o dump:on-app-using-req=80% ./my_program
    malt -o dump:on-app-using-req=4096K ./my_program
    malt -o dump:on-app-using-req=100M ./my_program
    malt -o dump:on-app-using-req=10G ./my_program

Option `dump:on-thread-stack-using`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Will dump if one of the thread stack reach the given limit. The value is given in % of the global memory
available or in K, M, G. Empty to disable (default).

**Default**: empty

.. code-block:: shell

    malt -o dump:on-thread-stack-using= ./my_program
    malt -o dump:on-thread-stack-using=80% ./my_program
    malt -o dump:on-thread-stack-using=4096K ./my_program
    malt -o dump:on-thread-stack-using=100M ./my_program
    malt -o dump:on-thread-stack-using=10G ./my_program

Option `dump:on-alloc-count`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Dump when number of allocations reach limit in G, M, K (empty to disable).

**Default**: empty

.. code-block:: shell

    malt -o dump:on-alloc-count= ./my_program
    malt -o dump:on-alloc-count=1000 ./my_program
    malt -o dump:on-alloc-count=1K ./my_program
    malt -o dump:on-alloc-count=1M ./my_program
    malt -o dump:on-alloc-count=1G ./my_program

Option `dump:watch-dog`
^^^^^^^^^^^^^^^^^^^^^^^

Will start a thread which will spy the memory usage of the process and trigger the dump as soon as it sees it
going to hight. This is to balance the fact than normally the system and process memory is spied only sometimes
by MALT in normal condition to keep the overhead low.

**Default**: false

.. code-block:: shell

    malt -o dump:watch-dog=false ./my_program
    malt -o dump:watch-dog=true ./my_program

Section `python`
----------------

The `python` section permit to configure how to instrument python. Notice that you need to build MALT with
`--enable-python` so it is effective.

Option `python:enabled`
^^^^^^^^^^^^^^^^^^^^^^^

Enable of disable the python instrumentation.

**Default**: true

.. code-block:: shell

    malt -o python:enabled=true ./my_program
    malt -o python:enabled=false ./my_program

Option `python:stack`
^^^^^^^^^^^^^^^^^^^^^

Select the stack instrumentation mode, either `enter-exit`, `backtrace` or `none`. By default `enter-exit` is
faster so you should use it. When enabling sampling you need to use `bactrace` if you don't want to pay
an unneeded overhead. You can also disable python stack checking with `none`.

**Default**: enter-exit

.. code-block:: shell

    malt -o python:stack=enter-exit ./my_program
    malt -o python:stack=backtrace ./my_program
	malt -o python:stack=none ./my_program

Option `python:mix`
^^^^^^^^^^^^^^^^^^^

By default when disabled the C & Python stacks are analysed independently which mean that is a python
function call a C function you will see only the C call stack. Mix allow to merge the two layeres so you
see that python call C. But it adds overhead on the anlysis of course because of the extra work.

**Default**: false

.. code-block:: shell

    malt -o python:mix=false ./my_program
    malt -o python:mix=true ./my_program

Option `python:obj`
^^^^^^^^^^^^^^^^^^^

Analyse or ignore the object allocation domain of python. This is interesting to ignore all the small
allocs used by the language as it would have been on the stack in C. It improves a lot the profiling performance
of python but miss part of the memory consumption if your program store lots of small objets for long times.

**Default**: true

.. code-block:: shell

    malt -o python:obj=true ./my_program
    malt -o python:obj=false ./my_program

Option `python:mem`
^^^^^^^^^^^^^^^^^^^

Same but for the mem allocation domain of python. In principle you should let it enabled.

**Default**: true

.. code-block:: shell

    malt -o python:mem=true ./my_program
    malt -o python:mem=false ./my_program

Option `python:raw`
^^^^^^^^^^^^^^^^^^^

Same but for the raw allocation domain of python which is backed by the standard C malloc function. In principle you should let it enabled.

**Default**: true

.. code-block:: shell

    malt -o python:raw=true ./my_program
    malt -o python:raw=false ./my_program

Option `python:hide-imports`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Hide the stack of memory allocations done by imports. It resude the noice in
analysis, the overhead and the size of the profile file.

If you need to look at the details of imports, you should set it to false.

If enabled, you will see in the call stack the entry `MALT_PYTHON_HIDEN_IMPORTS`.

**Default**: true

.. code-block:: shell

    malt -o python:hide-imports=true ./my_program
    malt -o python:hide-imports=false ./my_program

Option `python:mode`
^^^^^^^^^^^^^^^^^^^^

Define the python instrumentation mode which define how to get the line number.

The value can be :

- **profile** for using the profiling mode, which means we need extra calls
  on every malloc to get the line number.
- **trace** for using the python tracing mode so we get the line number directly.
  This second mode is currently experimental.

**Default**: profile

.. code-block:: shell

    malt -o python:mode=profile ./my_program
    malt -o python:mode=trace ./my_program

Section `c`
-----------

Option `c:malloc`
^^^^^^^^^^^^^^^^^

Track the C `malloc` calls.

**Default**: true

.. code-block:: shell

    malt -o c:malloc=true ./my_program
    malt -o c:malloc=false ./my_program

Option `c:mmap`
^^^^^^^^^^^^^^^

Track the direct C `mmap` calls.

**Default**: true

.. code-block:: shell

    malt -o c:mmap=true ./my_program
    malt -o c:mmap=false ./my_program

Section `tools`
---------------

The `tools` section permit to configure some of the sub-tools called by MALT to perform its analysis.

Option `tools:nm`
^^^^^^^^^^^^^^^^^

Use to extract the source location of the global variables. If true (default) it is used, otherwise it is skiped.

**Default**: true

.. code-block:: shell

    malt -o tools:nm=true ./my_program
    malt -o tools:nm=false ./my_program

Option `tools:nm-max-size`
^^^^^^^^^^^^^^^^^^^^^^^^^^

By default it limits the size of the .so files on which to apply NM in order to keep a decent profile dumping
time when running on large frameworks like PyTorch which tends to load huge .so files in memory.

**Default**: 50M

.. code-block:: shell

    malt -o tools:nm-max-size=50M ./my_program
    malt -o tools:nm-max-size=50M ./my_program

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
    points=1000            ; keep 1000 points
    linar-index=false      ; use action ID instead of time

    [stack]
    enabled=true           ; enable stack profiles
    mode=backtrace         ; select stack tracing mode (backtrace|enter-exit)
    resolve=true           ; Automatically resolve symbols with addr2line at exit.
    libunwind=false        ; Enable of disable usage of libunwind to backtrace.
    skip=4                 ; Number of stack frame to skip in order to cut at malloc level
    sampling=false         ; Sample and instrument only some stack.
    samplingBw=4093        ; Instrument the stack when seen passed 4K-3 bytes of alloc requests.

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
    instru=true            ; Enable of disable python instrumentation.
    stack=enter-exit       ; Select the Python stack instrumentation mode (backtrace, enter-exit, none).
    mix=false              ; Mix C stack with the python ones to get a uniq tree instread of two distincts
                           ; (not this adds overhead).
    obj=true               ; Instrument of not the OBJECT allocator domain of python.
    mem=true               ; Instrument of not the MEM allocator domain of python.
    raw=true               ; Instrument of not the RAW allocator domain of python.

    [tools]
    nm=true                ; Enable usage of NM to find the source locatoin of the global variables.
    nmMaxSize=50M          ; Do not call nm on .so larger than 50 MB to limit the profile dump overhead.

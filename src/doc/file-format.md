MALT Output Format
==================

MALT support two output format, a first one compatible with callgrind/kcachegrind syntax
which is described in <http://valgrind.org/docs/manual/cl-format.html>. The second format
is based on JSON and related to MALT.

**Remark**: There was a second version of MALT started at some point with
a cleaner JSON format but I never had time to adapt the GUI letting
the new backend pending for latter. The current (v1) version has a
global layout far less clean with some topology mistakes in the schema
to be improved.

Global structure
----------------

The json file contains major structures as :

```json5
{
    /* Provide some global informations about the run. */
    "run": {},
    /* Provide a dump of the MALT configuration. */
    "config": {},
    /* Provide profile projected on call stacks */
    "stacks": {},
    /* Provide information to map addresses to source code. */
    "sites": {},
    /* Provide metrics profile over time. */
    "timeline": {},
    /* Provide scatter metrics by sampling over time, lifetime and size */
    "scatter": {},
    /* Provide per thread metrics. */
    "threads": {},
    /* Some statistic represented by distributions about memory allocations . */
    "memStats": {},
    /* Provide some global metric related to the run. */
    "globals": {},
    /* Provide the list of leaks seen at the end of execution (blocks still allocated). */
    "leaks": {}
}
```

About `run` section
---------------------------

This section provide a global summary to give meta informations about the run.

```json5
"run":{
    /* MALT json format version */
    "formatVersion":"1.1",
    /* Tools and version used to generate the file */
    "tool":"malt-0.0.0",
    /* Date and time of the run (profile generation time, so exit time) */
    "date":"2014-11-01 18:56",
    /* Total exacution time from start to profile dump in CPU cycles */
    "runtime":416204055,
    /* Executable name */
    "exe":"simple-case-finstr-linked",
    /* Exact exacuted command with arguments */
    "command":"./simple-case-finstr-linked",
    /* Host where the run was done */
    "hostname":"my-host",
    /* Library of allocator in use. */
    "allocator": "/usr/lib/libc.so.2.6"
},
```

About `config` section
----------------------

This section provide a dump of the config setup used to configure MALT for the
run. This is more for debugging purpose but can also be used to keep track
of valid options for cases.

```json5
{
    "config": {
        "time": {
        "enabled": true,
        "points": 512,
        },
        "stack": {
        "enabled": true,
        },
        /* ..... */
        /* look on the user guide to get the detailed config option list */
    }
}
```

About `stacks` section
----------------------

The `stack` entry contain all the statistics attached to each call stacks.
It is build linking a call stack (with functions pointers) to the stat object.

For dyanmic libraries (`.so` files) this is the absolute address at runtime
which if you want to address in the library itself removing the shift
should be recomputed but this is not required as the `sites` section already
has all the conversion to code locations. Consider the address more like a
key.

**Rational:** we used the absolute runtime address because it can match easily 
with what we can dump with 0 cost into the trace mode (not yet propagated
to the view but done in backen).

```json5
{
    /* Stack root object */
    "stacks": {
        /* List of stack stats */
        "stats": [
        {
            /* stack trace with stack function pointers */
            "stack": [
            /* leaf call */
            "0x7f36c104cd72",
            "0x7f36c1056885",
            "0x402d78",
            "0x7f36c096452a",
            /* root call */
            "0x402209"
            ],
            /* Stack idendifier to be possibly reuse in time charts to know
            the hit point or used in trace files */
            "stackId": "0x7f36c0fda660",
            /* Informations attached to the call stack */
            "infos": {
            /* Number of malloc(0) */
            "countZeros": 0,
            /* Max number of alive malloced segments */
            "maxAliveReq": 16,
            /* Alive requested at profile dump time (leaks) */
            "aliveReq": 0,
            /* Allocation properties */
            "alloc": {
                /* Number of allocations */
                "count": 1,
                /* Minimal size */
                "min": 16,
                /* Maximal size */
                "max": 64,
                /* Sum of sizes of all allocated buffers over the whole run
                (cumulated allocations).
                */
                "sum": 80
            },
            /* Free statistics */
            "free": {
                /* Number of allocations */
                "count": 0,
                /* Minimal size */
                "min": 0,
                /* Maximal size */
                "max": 0,
                /* Sum of sizes of all freed buffers over the whole run
                (cumulated allocations).
                */
                "sum": 0
            },
            /* Elapsed time between malloc and free in CPU cycles */
            "lifetime": {
                /* Number of allocations to compute average */
                "count": 2,
                /* Minimal lifetime */
                "min": 24423,
                /* Maximal lifetime */
                "max": 44423,
                /* Sum of all lifetime to compute agerage */
                "sum": 68846
            },
            /* Memory used at ths location at peak time (alived memory) */
            "globalPeak": 0,
            /* Number of reallc() calls */
            "reallocCount": 0,
            /* Sum of relaloc delta to compute averahe growth/reduction. */
            "reallocSumDelta": 0
            }
        },
        ],
    }
}
```

About `sites` section
---------------------

This section contain the details of all call sited used in the `stacks` section
and identified by the address of the function.

This section contain many sub sections.

```json5
{
    /*
    Dump in a json format of /proc/self/maps to reconstruct addresses
    ni post mortem if really neaded.
    */
    "map": [],
    /*
    List of strings (function names, files) not to repeat thousand times.
    and keep file small
    */
    "strings": [],
    /*
    Instruction list indexed by addressed and giving the souce code
    location (file, line...)
    */
    "instr": {}
}
```

Details about `sites.map`
-------------------------

List of entries of `/prof/self/maps`.

```json5
{
    "sites": {
        "maps": [
            {
                /* base address of the section */
                "lower": "0x400000",
                /* top address of the section */
                "upper": "0x402000",
                /* Offset in file (currently can conatin '(nil)', need to fix to remove this) */
                "offset": "0x2000",
                /* binary file */
                "file": "/home/sebv/Projects/malt/build/src/lib/tests/simple-case-finstr"
            },
            {},
            {},
            //.....
        ]
    }
}
```

About `sites.strings` section
-----------------------------

This section is an array of strings avoiding to copy the repeated strings
too many time for instruction source locations (eg. file names....). This
reduce a lot the size of the profile file.

The index in the array is the key identifying the string and being used into
the `instr` section entries just silimarly to a pointer.

```json5
{
    "sites": {
        "strings": [
            /* The first one is always this one to be used for unknown values. */
            "??",
            /*
            This one is also always present in order to represent the root of
            all call stacks.
            */
            "[0x1]",
            "/home/USER/Projects/malt/src/lib/tests/simple-case.cpp",
            "funcC()",
        ]
    }
}
```

About `sites.instr` section
---------------------------

This section list all the instruction addresses needed to run over the
profile and match them to the source code location if debug symbols has
been activated.

Notice strings are identified as a key to be found in the `sites.strings`
section in order to reduce the size of the section avoiding them copying
to many time and get a smaller file.

```json5
{
    "sites": {
        "instr": {
        /* Instruction indentified by its address */
        "0x1": {
            "function": 71
        },
        "0x402320": {
            /* ID in 'site.strings' of file path */
            "file": 1,
            /* ID in 'site.strings' of function name */
            "function": 3,
            /* Line in file */
            "line": 124,
            /* ID in 'site.strings' of binary file path */
            "binary": 1
        }
    }
}

```

About `timeline` section
------------------------

This section contain the dynamic aspects of the malloc tracking via time
lines by sampling malloc calls over time ranges.

The global abstract representation of those data is based on a sampling
of the total execution to split it into a vertain number of points which
is configured by config file (`time.points` being 512 by default).

Here all clocks will be based on CPU ticks, this not the more previse
and monotonic source but is fine enougth to permit to get fast acces
to a precise clock. The conversion frequency can be found in the
`global.ticksPerSecond` section to make conversion.

*Experimental*: Noticed that on each interval we kept the max observed values,
eg max allocated memory, not the exact value. You will also
noticed the json provide the callsite being currently experimental
to be able to say what was the called function called when we
reached to peak in order to know which portion of the code
triggered it. This is not yet reported into the GUI.

The global content layout of this section correspond to:

```json5
{
    "timeline": {
        /* memory usage timeline (allocs) */
        "memoryTimeline": {},
        /* system memory usage timeline */
        "systemTimeline": {},
        /* memory allocation rates/bandwidth */
        "memoryBandwidth": {},
    }
}
```

Detailing `memoryTimeline` it gives:

```json5
{
    "timeline": {
        /* Memory timeline section */
        "memoryTimeline": {
            /*
            Remember absolute start point, this is more if ever the user
            want latter to compare with multiple runs on the same machine
            but that does not consider reboot of other server which might
            have a different value.
            */
            "start": 52892167387036,
            /* Number of cycles considered between two time points. */
            "perPoints": 57728,
            /* Peak measurement to get larget values obversed over the run. */
            "peak": [
                /* Requested memory */
                1146605,
                /* Physical memory */
                515485696,
                /* Virtual meomry */
                6369280,
                /* Internal memory */
                97144,
                /* Number of active segements */
                163
            ],
            /*
            Now the timed value, each sub entry is a time step with all attached
            values.
            */
            "values": [
                [
                    /* Requested memory */
                    72704,
                    /* Physical memory */
                    16171008,
                    /* Virtual meomry */
                    4112384,
                    /* Internal memory */
                    20704,
                    /* Number of active segements */
                    1
                ],
            ],
            /* Callsite triggering the max value on each interval (experimental) */
            "callsite": [
                "0x7f36c0fd9f60",
                "(nil)",
                "0x7f36c0fe1258",
            ]
        }
    }
}
```

The `systemTimeline` provide very similar information by tracking the state
of the global memory available or used by the full system.

```json5
{
    "timeline": {
        "systemTimeline": {
            "start":...,
            "perPoints":...,
            "peak": [
                /* Free memory */
                4573065216,
                /* Cached (FS) memory */
                0,
                /* Swaped memory */
                8713916416
            ],
            "values": [
                [
                    4573065216,
                    0,
                    8713916416
                ],
                [],
                [],
            ]
        }
    }
}
```

The `memoryBandwidth` provide some metrics about how much memory we
allocate/deallocate per second of number of memory managment operations
we are doing per seconds. This permits detecting possibly bad patterns
pushing too much presure to the underlying operating system.

```json5
{
    "timeline": {
        "memoryBandwidth": {
            "start":...,
            "perPoints":...,
            "peak": [
                /* Allocated memory */
                382,
                /* Allocation count */
                3931828,
                /* Freed memory */
                107,
                /* Freed segments */
                2134292
            ],
            "values": [
                [
                    1,
                    72704,
                    0,
                    0
                ],
                [],
                [],
            ]
        }
    }
}
```

About the `scatter` section
---------------------------

This section try to extract 2D representation of the memory parameter space
by extraction two axis information in a sampled way based by default
on logarithmic sampled region. It provides multiple data sets.

It splitts in multiple sub-sections:

* `sizeOverTime`: Provide allocation sizes used over time to detect
if there is phases with small then large allocations...
* `lifetimeOverSize`: Provide allocation lifetimes over size to know
if we allocate mostly large chunks with long life of small with short
or... to better found wrong performance pattern.

```json5
{
    "scatter": {
        "sizeOverTime": {},
        "lifetimeOverSize": {},
    }
}
```

In both case, the content follow the given structuration:

```json5
{
    "scatter": {
        "sizeOverTime": {
            /* Meta informations to interpret datas */
            "infos": {
                /* Infos on X axis */
                "xAxis": {
                    /* Value range goes from 0 to max */
                    "max": 67108864,
                    /* Numver of entries on X axis is 64 */
                    "size": 64,
                    /* Ranges are built with log2 base, not linear. */
                    "log": false
                },
                /* Same for Y axis*/
                "yAxis": {
                    "max": 2097152,
                    "size": 64,
                    "log": true
                }
            },
            /* List of 2D points to cover space (only if no 0) */
            "points": [
                /* Define a new point */
                {
                    /* X axis location */
                    "x": 13631488,
                    /* y axis location */
                    "y": 1,
                    /* value attached to the location */
                    "v": 1
                },
                {},
            ],
        },
    }
}
```

About `threads` section
-----------------------

This section provide statistics about thread memory operation distribution.
It contains for every thread a list of values:

* `stackMem`: Memory used over time for the call stack of the thread. This
 is more an experimental feature.
* `cntMemOps`: Absolute number of memory operations (allocation/free) done
 by the thread over the whole run.
* `stats`: Detailed per memory management statistics.

It gives with an example:

```json5
{
    /* Infos about threads */
    "threads": [
        /* first thread */
        {
        /* Stats about stack memory size of thread */
        "stackMem": {},
        /* Number of memory management operations */
        "cntMemOps": 219,
        /* Stats per memoruy operation kind */
        "stats": {},
        },
        {},
    ]
}
```

Given details on `stackMem`:

```json5
{
    /* Infos about threads */
    "threads": {
        /* Memory used by stack of thread **/
        "stackMem": {
            /* Max size of stack */
            "size": 1152,
            /* Stack details (call site pointers) */
            "stack": [
                "0x402730",
                "0x402730",
                /* root of call stack entry point */
                "0x1",
            ],
            /* Stack memory for each stack level from root to end */
            "mem": [
                1152,
                1120,
                0,
            ],
            /* TODO ?? forgot, looks redundant with size */
            "total": 0,
            /* Provide stack size tracking over time to get dynamic */
            "timeprofiler": {
                /* Minimal size for each sampled point */
                "min": [],
                /* Maximal size for each sampled point */
                "max": [],
                /* Index (can be sparse if thread was sleeping of not started) */
                "index": [],
                /* Callsite on max size reach point for each range */
                "location": [],
                /*
                Timesamp associated with each index, starting from 0
                at application start time
                */
                "timestamp": [],
                /* When the peak appear */
                "peakTimestamp": 37725894,
                /* Memory on peak time */
                "peakMemory": 1152,
                /* Index of peak */
                "peakIndex": 369,
                /* Do we use linear index. (TODO describe) */
                "linearIndex": true
            },
        }
    }
}
```

The `stats` section itself contains:

```json5
{
    "thread": [
        {
            /* Statistics about memory functions usage */
            "stats": {
                /* Calls of libc malloc() */
                "malloc": {
                    /* Number of calls */
                    "count": 6,
                    /* Total memory allocated by all calls */
                    "sum": 1309,
                    /* Time spent in malloc */
                    "time": 33498
                },
                "posix_memalign": {
                    "count": 0,
                    "sum": 0,
                    "time": 0
                },
                "aligned_alloc": {
                    "count": 0,
                    "sum": 0,
                    "time": 0
                },
                "memalign": {
                    "count": 0,
                    "sum": 0,
                    "time": 0
                },
                "valloc": {
                    "count": 0,
                    "sum": 0,
                    "time": 0
                },
                "pvalloc": {
                    "count": 0,
                    "sum": 0,
                    "time": 0
                },
                "free": {
                    "count": 3,
                    "sum": 0,
                    "time": 0
                },
                "calloc": {
                    "count": 0,
                    "sum": 0,
                    "time": 0
                },
                "realloc": {
                    "count": 1,
                    "sum": 32,
                    "time": 475
                }
            }
        }
    ]
}
```

About `memoryStats` section
---------------------------

This section provide hints about allocation sizes being used by the program.

It contains sub-sections:

* `sizeMap`: Map number of allocation done for each size.
* `reallocJump`: Permit to study realloc by tracking each realloc gaps used.
* `globalVariables`: Memory used by global variables.

This gives as summary top level:

```json5
{
    "memStats": {
        "sizeMap": {},
        "reallocJump": [],
        "globalVariables": {},
    }
}
```

Internally we can detail `sizeMap` with:

```json5
{
    "memStats": {
        "sizeMap": {
            "4": 1,
            "16": 13,
            "21": 8,
            "24": 1,
            "32": 8,
            "34": 1,
            "48": 7,
            "59": 1,
            "100": 8,
            "104": 31,
            "1048576": 1
        }
    }
}
```

The `reallocJump` section look like:

```json5
{
    "memStats": {
        "reallocJump": [
            /* first gap */
            {
                /* original size before realloc */
                "oldSize": 0,
                /* out size after realloc */
                "newSize": 32,
                /* number of occurences */
                "count": 7
            }
        ]
    }
}
```

The `globalVariables` section then looks like:

```json5
{
    "memStats": {
        "globalVariables": {
            /* binary file */
            "/...../simple-case-finstr": [
                {
                    /* Name of constant */
                    "name": "gblString",
                    /* Memory size (bytes), not account TLS copies. */
                    "size": 25,
                    /*
                    Is it a TLS or not to know if we have one instance
                    per thread or not
                    */
                    "tls": false
                }
            ]
        }
    }
}
```

About `globals` section
-----------------------

This section summarize some global numbers about the run to be mostly reused
by other section to well interpret them.

```json5
{
    "globals": {
        /* Number of ticks per second for all timings to convert to seconds */
        "ticksPerSecond": 3411169769,
        /* Total available memory on the system */
        "totalMemory": 16798138368,
        /* Free available memory when starting the application */
        "freeMemoryAtStart": 4573065216,
        /* Cached memory when starting the application */
        "cachedMemoryAtStart": 8713916416,
        /* Maximum number of threads */
        "maxThreadCount": 8
    },
}
```

About `leaks` section
---------------------

This section list all the still alive chunks at exit time which are
reported here as leaks. This is a list of allocated memory per
call stacks.

```json5
{
    "leaks": [
        {
            /* Stack of allocation */
             "stack": [
                "0x7f36c104cd72",
                "0x7f36c1056885",
                "0x7f36c0e57e77",
                "0x7f36c1358179",
                "0x7f36c1358275",
                "0x7f36c134a0c9"
            ],
            /* Number of chunks */
            "count": 1,
            /* Total leaked memory */
            "memory": 72704
        }
    ]
}
```

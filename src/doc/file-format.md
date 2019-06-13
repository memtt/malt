MALT Output Format
==================

MALT support two output format, a first one compatible with callgrind/kcachegrind syntax
which is described in <http://valgrind.org/docs/manual/cl-format.html>. The second format
is based on JSON and related to MALT.

Global structure
----------------

The json file contains major structures as :

```json
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

Details about run
-----------------

This section provide a global summary to give meta informations about the run.

```json
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
    "hostname":"my-host"
},
```

Config
------

This section provide a dump of the config setup used to configure MALT for the
run. This is more for debugging purpose but can also be used to keep track
of valid options for cases.

```json
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
```

Details about stacks
--------------------

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

```json
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
```

Details about sites
-------------------

This section contain the details of all call sited used in the `stacks` section
and identified by the address of the function.

This section contain many sub sections.

```json
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

Details about sites.map
-----------------------

List of entries of `/prof/self/maps`.

```json
[
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
```

Details about sites.strings
---------------------------

This section is an array of strings avoiding to copy the repeated strings
too many time for instruction source locations (eg. file names....). This
reduce a lot the size of the profile file.

The index in the array is the key identifying the string and being used into
the `instr` section entries just silimarly to a pointer.

```json
[
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
```

Details about sites.instr
-------------------------

This section list all the instruction addresses needed to run over the
profile and match them to the source code location if debug symbols has
been activated.

Notice strings are identified as a key to be found in the `sites.strings`
section in order to reduce the size of the section avoiding them copying
to many time and get a smaller file.

```json
{
    "0x1": {
        "function": 71
    },
    "0x402320": {
        "file": 1,
        "function": 3,
        "line": 124,
        "binary": 1
    }
}
```

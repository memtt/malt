MATT output format
==================

MATT support two output format, a first one compatible with callgrind/kcachegrind syntax
which is described in http://valgrind.org/docs/manual/cl-format.html. The second format
is based on JSON and related to MATT.

Global structure
----------------

The json file contain major structures as :

	{
		globals: {}  /* Provide some global metric related to the run. */
		stacks: {}   /* Provide profile projected on call stacks */
		leaks: {}    /* Provide the list of leaks seen at end of execution (blocks still allocated). */
		memStats: {} /* Some statistic represented by distributions about memory allocations . */
		threads: {}  /* Provide per thread metrics. */
		timeline: {} /* Provide metrics profile over time. */
		sites: {}    /* Provide informations to map addresses to source code. */
	}

Details about stacks
--------------------

The "stacks" entry contain structure like :

	{
		stats: [                            /* List of stacks with data */
			{
				stack : ['0xa','0xb'],      /* Stacks with 0xb called by 0xa */
				infos : {}
			},
			....
		]
	}

Details about map
-----------------

	{
		map: [                             /* Provide /proc/self/map */
			{lower:0xa,upper:0xb,offset:'(nil)',file:'path'},
			...
		]
		strings: {}                        /* List of strings referenced in 'instr' */
		instr: {
			'0xa': {
				file: 1,                   /* Id of string */
				function: 2,               /* Id of string */
				line: 36,
				binary: "path"
			}
			'0xb' : {},
			...
		}
	}


Details about timeline
----------------------

	{
		freeBandwidth: {}
		allocBandwith: {}
		segments: {}
		internalMem: {}
		virtualMem: {}
		physicalMem: {}
		requestedMem: {}
	}
	
All of them follow :

	{
		min: [],                 /* Min value on each interval */
		max: [],                 /* Max value on each interval */
		index: [],               /* Last linear index of interval */
		timestamp: [],           /* Last timestamp of interval */
		peakMemory:
		peakTimesteamp:
		peakIndex:
		linearIndex: false|true
	}

_______________________________________________________________
 - reallocJump : Provide the stats about realloc sizes
 - sizeMap : Provide the stats about sizes requested by user.
 
 - map
	- instr
	- strings
	- sites
 
TODO move ticks per seconds in globals.
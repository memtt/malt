Code entry points
=================

Core system
-----------

The main class to run MALT is defined into core/ and named AllocStackProfiler. This class provide the core mechanisms
to track the global memory state of the program. This class must be initialized into your event wrappers after
initializing the internal allocator :

	static gblAllocStackProfiler * profiler = NULL;
	static gblOptions * options = NULL;
	
	void wrapperInit(void)
	{
		//alloc stack profiler use it so need to init
		initInternalAlloc();
		
		//setup options
		options = new Options();
		
		//dynamic init as might append in unknown order, cannot count on library loading
		profiler = new AllocStackProfiler(options,STACK_MODE_BACKTRACE,true);
	}

Per thread tracker
------------------

In addition to the global state tracking, MALT use a per thread tracker allocated by the global one :

	static __thread tlsAllocStackProfiler = NULL;
	
	void myWrapperThreadInit(void)
	{
		tlsAllocStackProfiler = gblAllocStackProfiler->createLocalStackProfiler(true);
	}

You might prefer to send all event to the local state, it will propagate automatically to the global state.

Stack tree
----------

There is 2 stack tree representation but we currently only use the one based on STL::map plus Stack objects, so be
optimized for the backtrace mode.

Writing profiles
-----------------

Profiled are dumped on exit which must be notified to the global state object via :

	void myWrapperExit(void)
	{
		gblAllocStackProfiler->onExit();
	}

This method is not written to be called many times, maybe it can but it will use malloc so might change the memory
layout for the next events compared to standard execution.

Writing wrappers
-----------------

Event wrappers can be based on LD\_PRELOAD to override functions from libraries. In this case, you can implement your
wrappers as :

	void * wrappedFunction(....)
	{
		//check init of global and per thread states, also check loading of old lib symbols through dlsym().
		//caution dlsym() use calloc() so it required special tricks to wrap this method.
		MALT_WRAPPER_LOCAL_STATE_INIT
		
		//run the default function
		assert(gblState.status > ALLOC_WRAP_INIT_SYM);
		void * res = gblState.malloc(size);
		
		//profile, the macro check current init state to avoid infinit reentrant loop on init
		//and to profiler internal use of wrappedFunction if profiler use it.
		MALT_WRAPPER_LOCAL_STATE_ACTION(tlsAllocStackProfiler->onWrappedFunction(....));
		
		return res;
	}

Default wrapper
---------------

As LD\_PRELOAD is the default mode to use MALT the main wrapper is implemented into wrapper/AllocWrapper.cpp which
wrap the libc functions and made all the glue stuff for this mode.

Output
------

Outputting is managed by the global profiler state object in onExit() function member. It mostly uses the subclasses :

 - SymbolResolver to resolve symbols from addresses (currently use calls to add2line program from binutils).
 - ValgrindOutputter to write a KCachegrind compatible profile by converting the internal MALT representation of the stack
   tree.

Other revelant classes
----------------------

For profiling you also find classes to track particular things :

 - CallStackInfo : attached to every call stacks, it represent the profiled stated we tracked for each stack.
 - StackSizeTracer : profile tracking for stack memory size over time.
 - SimpleCallStackTracer : used to generate trace of events if enabled.
 - Classes from valprof : used to profile states over time.
 - Classes from tools : Provide wrapper to call extern tools/libs to extract information about source code.
 


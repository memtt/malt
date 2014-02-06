/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//standard
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <iostream>
//libc POSIX.1, here we use GNU specific RTLD_NEXT (need _GNU_SOURCE)
#include <dlfcn.h>
//intenrals
#include <portability/Mutex.hpp>
#include <portability/OS.hpp>
#include <common/CodeTiming.hpp>
#include <common/Debug.hpp>
#include "AllocStackProfiler.hpp"

/***************** USING NAMESPACE ******************/
using namespace MATT;

/*******************  FUNCTION  *********************/
/** Prototype of malloc function to get it into function pointer. **/
typedef void * (*MallocFuncPtr) (size_t size);
/** Prototype of free function to get it into function pointer. **/
typedef void   (*FreeFuncPtr)   (void * ptr);
/** Prototype of calloc function to get it into function pointer. **/
typedef void * (*CallocFuncPtr) (size_t nmemb,size_t size);
/** Prototype of realloc function to get it into function pointer. **/
typedef void * (*ReallocFuncPtr)(void * ptr,size_t size);

/********************  ENUM  ************************/
/**
 * Follow the current state of GlobalState structure. By default it start as not initialized
 * (ALLOC_WRAP_NOT_READY) and go to larger values up to ALLOC_WRAP_FINISH).
**/
enum AllocWrapperGlobalStatus
{
	/** 
	 *  Initial state setup by compiler. It geneate lazy init on fist usage (needed to to use of 
	 * GlobalState before entering into main() function). 
	**/
	ALLOC_WRAP_NOT_READY,
	/** Intermediate state used to take care of first calloc() answer due to usage into dlsym() function. **/
	ALLOC_WRAP_INIT_SYM,
	/** Intermediate state when initilazing profiler object. **/
	ALLOC_WRAP_INIT_PROFILER,
	/** Ready for use. **/
	ALLOC_WRAP_READY,
	/** Now finish, ignore future calls to not instrument them after call of atexit(). **/
	ALLOC_WRAP_FINISH
};

/*********************  STRUCT  *********************/
/**
 * Store the global state of alloc wrapper, mainly used to maintain access to the profiling object and
 * keep track of the old (glibc) malloc/calloc... symbols to forward the requests after registration of
 * the event.
 * 
 * CAUTION : the 'state' and 'lock' bariables really need to stay on the top of the structure due to
 * static initialization of global 'gblState' variable.
**/
struct AllocWrapperGlobal
{
	/** Current state in life cycle of the structure. CAUTION, need to stay at first position. **/
	AllocWrapperGlobalStatus status;
	/** 
	 * Mutex to protect the structure in multi-thread mode, only used to protect the init state. 
	 * CAUTION, need to star at second position. 
	**/
	MATT::StaticMutex lock;
	/** Pointer to the old (glibc) malloc symbol. **/
	MallocFuncPtr malloc;
	/** Pointer to the old (glibc) free symbol. **/
	FreeFuncPtr free;
	/** Pointer to the old (glibc) calloc symbol. **/
	CallocFuncPtr calloc;
	/** Pointer to the old (glibc) realloc symbol. **/
	ReallocFuncPtr realloc;
	/** Pointer to the profiler (use pointer due to use before main, only way to ensure init before first malloc usage). **/
	AllocStackProfiler * profiler;
	/** Function used to init the structure on first use. **/
	void init(void);
	/** Function to cleanup the structure at exit. **/
	static void onExit(int status,void * arg);
};

/********************  STRUCT  **********************/
/**
 * Manage the local data attached to each threads. 
 * The content use lazy initialization on first access thanks to the default 'false' 
 * value of then 'init' field.
**/
struct ThreadLocalState
{
	/** Object used to follow the local stack for the enter/exit mode. **/
	EnterExitCallStack * enterExitStack;
	/** 
	 * Avoid to instrument inner allocations, marked at 'true' when entering in first level malloc/free.... 
	 * It permit to use dynamic allocation inside instrumentation functions.
	 **/
	bool inUse;
	/**
	 * Remember the default init state to manage lazy initialization.**/
	bool initialized;
	
	/** Function used to init the structure on first use. **/
	void init(void);
};

/********************  GLOBALS  **********************/
/** Store the global state of allocator wrapper. **/
static AllocWrapperGlobal gblState = {ALLOC_WRAP_NOT_READY,MATT_STATIC_MUTEX_INIT,NULL,NULL,NULL,NULL};
/** Store the per-thread state of allocator wrapper. **/
static __thread ThreadLocalState tlsState = {NULL,false,false};
/** Temporary buffer to return on first realloc used by dlsym and split infinit call loops. **/
static char gblCallocIniBuffer[4096];

/*******************  FUNCTION  *********************/
/**
 * Check the stack mode to use by reading MATT_STACK environnement variable.
 * If MATT_STACK does not exist it use the value from config file (stack:mode).
 * 
 * Expected values are :
 *    - 'backtrace'  : use backtrace in malloc/calloc... calls to get the call stack.
 *    - 'enter-exit' : follow the call of each function to continuously follow the stack status 
 *                     thanks to -finstrument-function flag of GCC/CLANG/ICC.
**/
static StackMode getStackMode(Options & options)
{
	//default values
	const char * mode = getenv("MATT_STACK");
	StackMode ret = MATT::STACK_MODE_BACKTRACE;
	
	//if not env use config file
	if (mode == NULL)
		mode = options.stackMode.c_str();

	//switches
	if (mode == NULL)
	{
		ret = STACK_MODE_BACKTRACE;
	} else if (strcmp(mode,"backtrace") == 0 || strcmp(mode,"") == 0) {
		ret = STACK_MODE_BACKTRACE;
	} else if (strcmp(mode,"enter-exit") == 0) {
		ret = STACK_MODE_USER;
	} else {
		MATT_FATAL_ARG("Invalid mode in MATT_STACK environnement variable : '%1'! Supportted : backtrace | enter-exit.").arg(mode).end();
	}
	
	//ok done
	return ret;
}

/*******************  FUNCTION  *********************/
/**
 * Function in charge of the lazy initialization of the global state.
 * It take care of multiple and parallel calls in case of use in multi-thread context.
**/
void AllocWrapperGlobal::init(void )
{
	//secure in case of first call in threads
	gblState.lock.lock();

	if (gblState.status == ALLOC_WRAP_NOT_READY)
	{
		//update state
		gblState.status = ALLOC_WRAP_INIT_SYM;
		
		//search addresses
		gblState.malloc = (MallocFuncPtr)dlsym(RTLD_NEXT,"malloc");
		gblState.free = (FreeFuncPtr)dlsym(RTLD_NEXT,"free");
		gblState.calloc = (CallocFuncPtr)dlsym(RTLD_NEXT,"calloc");
		gblState.realloc = (ReallocFuncPtr)dlsym(RTLD_NEXT,"realloc");

		//init profiler
		gblState.status = ALLOC_WRAP_INIT_PROFILER;
		
		//load options
		Options options;
		const char * configFile = getenv("MATT_CONFIG");
		if (configFile != NULL)
			options.loadFromFile(configFile);
		
		//ok do it
		gblState.profiler = new AllocStackProfiler(options,getStackMode(options),true);
		
		//print info
		fprintf(stderr,"Start memory instrumentation of %s - %d by library override.\n",OS::getExeName().c_str(),OS::getPID());

		//register on exit
		on_exit(AllocWrapperGlobal::onExit,NULL);

		//final state
		gblState.status = ALLOC_WRAP_READY;
	}
	
	//secure in case of first call in threads
	gblState.lock.unlock();
}

/*******************  FUNCTION  *********************/
/**
 * Function to register into on_exit() one. It permit to cleanup memory and to flush
 * the extracted profile at the end of the execution.
 *
 * @param status Exit status of the application (return value of main()).
 * @param arg Unused argument, will be NULL.
**/
void AllocWrapperGlobal::onExit(int status,void * arg)
{
	if (gblState.status == ALLOC_WRAP_READY)
	{
		gblState.status = ALLOC_WRAP_FINISH;
		gblState.profiler->onExit();
		delete gblState.profiler;
	} else {
		gblState.status = ALLOC_WRAP_FINISH;
	}
}

/*******************  FUNCTION  *********************/
/**
 * Function used on lazy init of the TLS variable, it will be called on first use of the struct.
**/
void ThreadLocalState::init(void)
{
	//check errors
	assert(tlsState.enterExitStack == NULL);
	
	//mark as init to authorize but in use to authorise malloc without intrum and cut infinit call loops
	tlsState.initialized = true;
	tlsState.inUse = true;

	//check init
	if (gblState.status == ALLOC_WRAP_NOT_READY)
		gblState.init();
	
	//create the local chain
	EnterExitCallStack * stack = new EnterExitCallStack();
	stack->enterFunction((void*)0x1);
	
	//ok mark no in use, ready for instr
	tlsState.enterExitStack = stack;
	tlsState.inUse = false;
}

/*******************  FUNCTION  *********************/
/**
 * Wrapper of the malloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
 * 
 * @param size Size of the element to allocate.
 * @return Pointer to the allocated memory, NULL in case of error.
**/
void * malloc(size_t size)
{
	//get addr localy to avoid to read the TLS every time
	ThreadLocalState & localState = tlsState;
	
	//check init
	if ( ! localState.initialized )
		localState.init();

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	void * res = gblState.malloc(size);

	//profile
	if (!localState.inUse && gblState.status == ALLOC_WRAP_READY)
	{
		localState.inUse = true;
		CODE_TIMING("mallocProf",gblState.profiler->onMalloc(res,size,localState.enterExitStack));
		localState.inUse = false;
	}

	//return segment to user
	return res;
}

/*******************  FUNCTION  *********************/
/**
 * Wrapper of the free function to capture deallocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
 * 
 * @param ptr Pointer to the memory segment to released.
**/
void free(void * ptr)
{
	//get addr localy to avoid to read the TLS every time
	ThreadLocalState & localState = tlsState;
	
	//check init
	if ( ! localState.initialized )
		localState.init();

	//profile
	if (!localState.inUse && gblState.status == ALLOC_WRAP_READY)
	{
		localState.inUse = true;
		CODE_TIMING("freeProf",gblState.profiler->onFree(ptr,localState.enterExitStack));
		localState.inUse = false;
	}

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	gblState.free(ptr);
}

/*******************  FUNCTION  *********************/
/**
 * Wrapper of the calloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
 * 
 * @param nmemb Number of element to allocate.
 * @param size Size of the element to allocate.
 * @return Pointer to the allocated memory, NULL in case of error.
**/
void * calloc(size_t nmemb,size_t size)
{
	//get addr localy to avoid to read the TLS every time
	ThreadLocalState & localState = tlsState;
	
	//check init
	if ( ! localState.initialized )
		localState.init();

	//calloc need a special trick for first use due to usage in dlsym
	//this way it avoid to create infinite loop
	if (gblState.status == ALLOC_WRAP_INIT_SYM)
	{
		assert(sizeof(gblCallocIniBuffer) >= size);
		return gblCallocIniBuffer;
	}
	
	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	void * res = gblState.calloc(nmemb,size);

	//profile
	if (!localState.inUse && gblState.status == ALLOC_WRAP_READY)
	{
		localState.inUse = true;
		gblState.profiler->onCalloc(res,nmemb,size,localState.enterExitStack);
		localState.inUse = false;
	}

	//return result to user
	return res;
}

/*******************  FUNCTION  *********************/
/**
 * Wrapper of the realloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
 * 
 * @param ptr Address of the old segment to resize, NULL to allocate a new one.
 * @param size New requested size, 0 to free the given segment.
 * @return Pointer to the allocated memory, NULL in case of error.
**/
void * realloc(void * ptr, size_t size)
{
	//get addr localy to avoid to read the TLS every time
	ThreadLocalState & localState = tlsState;
	
	//check init
	if ( ! localState.initialized )
		localState.init();

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	void * res = gblState.realloc(ptr,size);
	
	//profile
	if (!localState.inUse && gblState.status == ALLOC_WRAP_READY)
	{
		localState.inUse = true;
		CODE_TIMING("rellocProf",gblState.profiler->onRealloc(ptr,res,size,localState.enterExitStack));
		localState.inUse = false;
	}
	
	return res;
}

/*********************  STRUCT  *********************/
//Juste to be sure of the C symbol naming.
extern "C" 
{
	void __cyg_profile_func_enter (void *this_fn,void *call_site);
	void __cyg_profile_func_exit  (void *this_fn,void *call_site);
}

/*********************  STRUCT  *********************/
/**
 * Symbol used to capture entering in function when working with GCC/CLANG/ICC 
 * -finstrument-function CFLAG.
 *
 * It need to enable STACK_MODE_USER on profiler object to be usefull. This flag
 * is enabled by setting environnement variable STACK_MODE to "enter-exit" before
 * loading the instrumentation library.
 *
 * @param this_fn Define the name of the current function to exit.
 * @param call_site Define the address which made the call and on which to return.
**/
void __cyg_profile_func_enter (void *this_fn,void *call_site)
{
	//get addr localy to avoid to read the TLS every time
	ThreadLocalState & localState = tlsState;
	
	//check init
	if ( ! localState.initialized )
		localState.init();
	
	localState.enterExitStack->enterFunction(this_fn);
}

/*********************  STRUCT  *********************/
/**
 * Symbol used to capture exit of function when working with GCC/CLANG/ICC 
 * -finstrument-function CFLAG. 
 * 
 * It need to enable STACK_MODE_USER on profiler object to be usefull. This flag
 * is enabled by setting environnement variable STACK_MODE to "enter-exit" before
 * loading the instrumentation library.
 *
 * @param this_fn Define the name of the current function to exit.
 * @param call_site Define the address which made the call and on which to return.
**/
void __cyg_profile_func_exit  (void *this_fn,void *call_site)
{
	//get addr localy to avoid to read the TLS every time
	ThreadLocalState & localState = tlsState;
	
	//check init
	if ( ! localState.initialized )
		localState.init();
	
	localState.enterExitStack->exitFunction(this_fn);
}

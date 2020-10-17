/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
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
//#include <thread>
//libc POSIX.1, here we use GNU specific RTLD_NEXT (need _GNU_SOURCE)
#include <dlfcn.h>
//glibc
#include <malloc.h>
//signal
#include <signal.h>
//intenrals
#include <common/Debug.hpp>
#include <portability/OS.hpp>
#include <portability/Mutex.hpp>
#include <common/CodeTiming.hpp>
#include <core/StackSizeTracker.hpp>
#include <profiler/AllocStackProfiler.hpp>
#include <profiler/LocalAllocStackProfiler.hpp>
//locals
#include "malt.h"

/***************** USING NAMESPACE ******************/
using namespace MALT;

/********************  MACRO  ***********************/
/** Manage init of local state and fetch TLS in local pointer. **/
#define MALT_WRAPPER_LOCAL_STATE_INIT \
	/*get addr localy to avoid to read the TLS every time*/ \
	ThreadLocalState & localState = tlsState; \
	 \
	/*check init */  \
	if ( tlsState.status == ALLOC_WRAP_NOT_READY ) \
		localState.init();\
	\
	bool isEnterExit = true;\
	if (localState.profiler != NULL) \
		isEnterExit = localState.profiler->isEnterExit(); \
	if (isEnterExit){};/*to remove, but add it add warning*/

/********************  MACRO  ***********************/
/** Check init status of local and global state and call enter/exit methods, then do requested action. **/
#define MALT_WRAPPER_LOCAL_STATE_ACTION(action)  \
	if (gblState.status == ALLOC_WRAP_READY && tlsState.status == ALLOC_WRAP_READY) \
	{ \
		void * retAddr;\
		if (isEnterExit)\
		{\
			retAddr =__builtin_extract_return_addr(__builtin_return_address(0)); \
			localState.profiler->onEnterFunc((void*)__func__,retAddr,true); \
		}\
		do{action;}while(0); \
		if (isEnterExit) \
			localState.profiler->onExitFunc((void*)__func__,retAddr,true); \
	}

/*******************  FUNCTION  *********************/
/** Prototype of malloc function to get it into function pointer. **/
typedef void * (*MallocFuncPtr) (size_t size);
/** Prototype of free function to get it into function pointer. **/
typedef void   (*FreeFuncPtr)   (void * ptr);
/** Prototype of calloc function to get it into function pointer. **/
typedef void * (*CallocFuncPtr) (size_t nmemb,size_t size);
/** Prototype of realloc function to get it into function pointer. **/
typedef void * (*ReallocFuncPtr)(void * ptr,size_t size);
/** Prototype of posix_memalign function to get it into function pointer. **/
typedef int (*PosixMemalignFuncPtr)(void **memptr, size_t alignment, size_t size);
/** Prototype of aligned_alloc function to get it into function pointer. **/
typedef void *(*AlignedAllocFuncPtr)(size_t alignment, size_t size);
/** Prototype of valloc function to get it into function pointer. **/
typedef void *(*VallocFuncPtr)(size_t size);
/** Prototype of memalign function to get it into function pointer. **/
typedef void *(*MemalignFuncPtr)(size_t alignment, size_t size);
/** Prototype of pvalloc function to get it into function pointer. **/
typedef void *(*PVallocFuncPtr)(size_t size);
/** Prototype of mmap function to get it into function pointer. **/
typedef void *(*MmapFuncPtr)(void *start,size_t length,int prot,int flags,int fd,off_t offset);
/** Prototype of munmap function to get it into function pointer. **/
typedef int (*MunmapFuncPtr)(void *start,size_t length);
/** Prototype of mremap function to get it into function pointer. **/
typedef int (*MremapFuncPtr)(void *old_address, size_t old_size , size_t new_size, int flags);


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
	/** This is used by enable/disable functions on TLS state to ignore part of the code in threads **/
	ALLOC_WRAP_DISABLED,
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
	MALT::StaticMutex lock;
	/** Pointer to the old (glibc) malloc symbol. **/
	::MallocFuncPtr malloc;
	/** Pointer to the old (glibc) free symbol. **/
	::FreeFuncPtr free;
	/** Pointer to the old (glibc) calloc symbol. **/
	::CallocFuncPtr calloc;
	/** Pointer to the old (glibc) realloc symbol. **/
	::ReallocFuncPtr realloc;
	/** Pointer to the old (libc) posix_memalign symbol. **/
	::PosixMemalignFuncPtr posix_memalign;
	/** Pointer to the old (libc) aligned_alloc symbol. **/
	::AlignedAllocFuncPtr aligned_alloc;
	/** Pointer to the old (libc) valloc symbol. **/
	::VallocFuncPtr valloc;
	/** Pointer to the old (libc) pvalloc symbol. **/
	::PVallocFuncPtr pvalloc;
	/** Pointer to the old (libc) memalign symbol. **/
	::MemalignFuncPtr memalign;
	/** Pointer to the old (libc) mmap symbol. **/
	::MmapFuncPtr mmap;
	/** Pointer to the old (libc) munmap symbol. **/
	::MunmapFuncPtr munmap;
	/** Pointer to the old (libc) mremap symbol. **/
	::MremapFuncPtr mremap;
	/** Pointer to the profiler (use pointer due to use before main, only way to ensure init before first malloc usage). **/
	::AllocStackProfiler * profiler;
	/** Keep track of user options. **/
	Options * options;
	/** Function used to init the structure on first use. **/
	void init(void);
	/** Function to cleanup the structure at exit. **/
	static void onExit(void);
};

/********************  STRUCT  **********************/
/**
 * Manage the local data attached to each threads. 
 * The content use lazy initialization on first access thanks to the default status.
 * value of then 'init' field.
**/
struct ThreadLocalState
{
	/** 
	 * Local profiler manager to track stacks and cache information before flushing them in the globoal one.
	 * It avoid to take locks every time and eventually permit to get a complete per-thread tracking.
	**/
	LocalAllocStackProfiler * profiler;

	/**
	 * Remember the default init state to manage lazy initialization.**/
	AllocWrapperGlobalStatus status;
	
	/** Function used to init the structure on first use. **/
	void init(void);
};

/********************  GLOBALS  **********************/
/** Store the global state of allocator wrapper. **/
static AllocWrapperGlobal gblState = {ALLOC_WRAP_NOT_READY,MALT_STATIC_MUTEX_INIT,NULL,NULL,NULL,NULL,NULL};
/** Store the per-thread state of allocator wrapper. **/
static __thread ThreadLocalState tlsState = {NULL,ALLOC_WRAP_NOT_READY};
/** Temporary buffer to return on first realloc used by dlsym and split infinit call loops. **/
static char gblCallocIniBuffer[4096];

/*******************  FUNCTION  *********************/
/**
 * Check the stack mode to use by reading MALT_STACK environnement variable.
 * If MALT_STACK does not exist it use the value from config file (stack:mode).
 * 
 * Expected values are :
 *    - 'backtrace'  : use backtrace in malloc/calloc... calls to get the call stack.
 *    - 'libunwind'  : similar to backtrace but use libunwind instead of the glibc implementation.
 *    - 'enter-exit' : follow the call of each function to continuously follow the stack status 
 *                     thanks to -finstrument-function flag of GCC/CLANG/ICC.
**/
static StackMode getStackMode(Options & options)
{
	//default values
	const char * mode = getenv("MALT_STACK");
	StackMode ret = MALT::STACK_MODE_BACKTRACE;
	
	//if not env use config file
	if (mode == NULL)
		mode = options.stackMode.c_str();

	//switches
	if (mode == NULL)
	{
		ret = STACK_MODE_BACKTRACE;
	} else if (strcmp(mode,"backtrace") == 0 || strcmp(mode,"") == 0) {
		ret = STACK_MODE_BACKTRACE;
	} else if (strcmp(mode,"libunwind") == 0 || strcmp(mode,"") == 0) {
		ret = STACK_MODE_BACKTRACE;
		options.stackLibunwind = true;
	} else if (strcmp(mode,"enter-exit") == 0) {
		ret = STACK_MODE_ENTER_EXIT_FUNC;
	} else {
		MALT_FATAL_ARG("Invalid mode in MALT_STACK environnement variable : '%1'! Supported : backtrace | enter-exit.").arg(mode).end();
	}
	
	//ok done
	return ret;
}

/*******************  FUNCTION  *********************/
static void maltDumpOnEvent(void)
{
	//stop instr & dump
	if (gblState.status == ALLOC_WRAP_READY)
	{
		gblState.status = ALLOC_WRAP_FINISH;
		gblState.profiler->onExit();

		//TODO restore to pursue profiling
	}
}

/*******************  FUNCTION  *********************/
static void maltSigKillHandler(int s)
{
	fprintf(stderr,"MALT: Capture signal KILL, dump profile and exit.");
	exit(1);
}

/*******************  FUNCTION  *********************/
static void maltSigHandler(int signum)
{
	//twik print
	std::string name = OS::getSignalName(signum);

	//print
	fprintf(stderr,"MALT: Capture signal %s, dump profile and continue without profiling.\n", name.c_str());

	//dump
	maltDumpOnEvent();
}

/*******************  FUNCTION  *********************/
void maltSetupSigHandler(const Options & options)
{
	//get list
	std::istringstream split(options.dumpOnSignal);
	std::vector<std::string> tokens;
	for (std::string each; std::getline(split, each, ','); tokens.push_back(each));

	//loop on all
	for (std::vector<std::string>::const_iterator it = tokens.begin() ; it != tokens.end() ; ++it)
	{
		//debug
		if (gblOptions->outputVerbosity != MALT_VERBOSITY_SILENT)
			fprintf(stderr, "MALT: will dump on signal %s (caution, can do only one dump, bugfix in dev !)\n", it->c_str());
		
		//setup
		if (*it == "help" || *it == "avail")
			OS::printAvailSigs();
		else if (*it == "SIGINT")
			OS::setSigHandler(maltSigKillHandler,SIGINT);//TODO check if we keep this !
		else
			OS::setSigHandler(maltSigHandler,*it);
	}
}

/*******************  FUNCTION  *********************/
void * maltDumpAfterSecondsThreadMain(void * arg)
{
	//extract
	int secs = (size_t)arg;

	//we will need it latter to onExit
	tlsState.init();

	//wait
	sleep(secs);

	//dump
	maltDumpOnEvent();

	//done
	return NULL;
}

/*******************  FUNCTION  *********************/
void maltDumpAfterSecondsThread(const Options & options)
{
	//extract
	size_t secs = options.dumpAfterSeconds;

	//check
	if (secs == 0)
		return;
	
	//spawn thread (c++11 style)
	/*std::thread th([secs] { 
		maltDumpAfterSecondsThreadMain();
	});*/

	//detach (c+11 style)
	//th.detach();

	//spawn threads
	pthread_t th;
	int rc = pthread_create(&th, NULL, maltDumpAfterSecondsThreadMain, (void*)secs);
	assumeArg(rc == 0, "Fail to start sleep thread for timeout dump: %1").argStrErrno().end();
}

/*******************  FUNCTION  *********************/
extern "C" {

/*******************  FUNCTION  *********************/
int maltInitStatus(void)
{
	return 1;
}
	
void maltEnable(void)
{
	/*get addr localy to avoid to read the TLS every time*/
	ThreadLocalState & localState = tlsState;

	/*check init */ 
	if ( localState.status == ALLOC_WRAP_NOT_READY )
		localState.init();
	
	if(localState.status == ALLOC_WRAP_DISABLED)
		localState.status = ALLOC_WRAP_READY;
}

/*******************  FUNCTION  *********************/
void maltDisable(void)
{
	/*get addr localy to avoid to read the TLS every time*/
	ThreadLocalState & localState = tlsState;

	/*check init */ 
	if ( localState.status == ALLOC_WRAP_NOT_READY )
		localState.init();
	
	if(localState.status == ALLOC_WRAP_READY)
		localState.status = ALLOC_WRAP_DISABLED;
}
	
}

/*******************  FUNCTION  *********************/
/**
 * Function in charge of the lazy initialization of the global state.
 * It take care of multiple and parallel calls in case of use in multi-thread context.
**/
void AllocWrapperGlobal::init(void )
{
	//skip if filtered exe
	bool skip = false;

	//secure in case of first call in threads
	gblState.lock.lock();

	if (gblState.status == ALLOC_WRAP_NOT_READY)
	{
		//update state
		gblState.status = ALLOC_WRAP_INIT_SYM;
		
		//search addresses
		gblState.mmap = (::MmapFuncPtr)dlsym(RTLD_NEXT,"mmap");
		gblState.munmap = (::MunmapFuncPtr)dlsym(RTLD_NEXT,"munmap");
		gblState.malloc = (::MallocFuncPtr)dlsym(RTLD_NEXT,"malloc");
		gblState.free = (::FreeFuncPtr)dlsym(RTLD_NEXT,"free");
		gblState.calloc = (::CallocFuncPtr)dlsym(RTLD_NEXT,"calloc");
		gblState.realloc = (::ReallocFuncPtr)dlsym(RTLD_NEXT,"realloc");
		gblState.posix_memalign = (::PosixMemalignFuncPtr)dlsym(RTLD_NEXT,"posix_memalign");
		gblState.aligned_alloc = (::AlignedAllocFuncPtr)dlsym(RTLD_NEXT,"aligned_alloc");
		gblState.valloc = (::VallocFuncPtr)dlsym(RTLD_NEXT,"valloc");
		gblState.memalign = (::MemalignFuncPtr)dlsym(RTLD_NEXT,"memalign");
		gblState.pvalloc = (::PVallocFuncPtr)dlsym(RTLD_NEXT,"pvalloc");
		gblState.mremap = (::MremapFuncPtr)dlsym(RTLD_NEXT,"mremap");

		//init profiler
		gblState.status = ALLOC_WRAP_INIT_PROFILER;
		
		//init internal alloc
		initInternalAlloc(true);

		//load options
		gblState.options = &initGlobalOptions();
		const char * configFile = getenv("MALT_CONFIG");
		if (configFile != NULL)
			gblState.options->loadFromFile(configFile);
		const char * envOptions = getenv("MALT_OPTIONS");
		if (envOptions != NULL)
			gblState.options->loadFromString(envOptions);
		
		//adapt stack skip mode (to adapt malt to compiler inlinling)
		BacktraceStack testStack;
		int stackDelta = testStack.getBactraceSkipOptimDelta();
		gblState.options->stackSkip += stackDelta;
		
		//extract stack mode and update options for libunwind if required
		StackMode mode = getStackMode(*gblState.options);
		
		//ok do it
		gblState.profiler = new AllocStackProfiler(*gblState.options,mode,true);
		gblState.profiler->setRealMallocAddr(gblState.malloc);

		//filter exe
		if (gblState.options->exe.empty() == false && OS::getExeName() != gblState.options->exe)
		{
			fprintf(stderr,"MALT: skip %s != %s\n",OS::getExeName().c_str(),gblState.options->exe.c_str());
			skip = true;
		}
		
		//print info
		if (gblOptions->outputVerbosity >= MALT_VERBOSITY_DEFAULT && !skip)
			fprintf(stderr,"MALT: Start memory instrumentation of %s - %d by library override.\n",OS::getExeName().c_str(),OS::getPID());
		
		//disable childs
		if (gblOptions->childs == false)
			unsetenv("LD_PRELOAD");

		//register on exit
		//TODO remove when ensure that the attribute method work
		//This line tend to create deadlock due to reentrance for some libs (openmpi for example)
		//atexit(AllocWrapperGlobal::onExit);
		
		//register sigkill handler & dump sleep
		maltSetupSigHandler(*gblOptions);
		maltDumpAfterSecondsThread(*gblOptions);

		//final state
		gblState.status = ALLOC_WRAP_READY;

		//skip
		if (skip)
			gblState.status = ALLOC_WRAP_FINISH;
	}

	//secure in case of first call in threads
	gblState.lock.unlock();
}

/*******************  FUNCTION  *********************/
void libdestructor(void) __attribute__((destructor (101)));
void libdestructor(void)
{
	AllocWrapperGlobal::onExit();
}

/*******************  FUNCTION  *********************/
/**
 * Function to register into on_exit() one. It permit to cleanup memory and to flush
 * the extracted profile at the end of the execution.
 *
 * @param status Exit status of the application (return value of main()).
 * @param arg Unused argument, will be NULL.
**/
//We use destructor attribute to register and provide a small value for optional
//priority arg, so it might be the last library to destruct to capture as much
//free() as possible for the leak tracking.
void AllocWrapperGlobal::onExit(void)
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
	assert(tlsState.profiler == NULL);
	
	//mark as init to authorize but in use to authorise malloc without intrum and cut infinit call loops
	tlsState.status = ALLOC_WRAP_INIT_PROFILER;

	//check init
	if (gblState.status == ALLOC_WRAP_NOT_READY)
		gblState.init();
	
	//create the local chain
	tlsState.profiler = gblState.profiler->createLocalStackProfiler(true);
	
	//mark ready
	if (gblOptions->enabled)
		tlsState.status = ALLOC_WRAP_READY;
	else
		tlsState.status = ALLOC_WRAP_DISABLED;
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
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	
	ticks t = getticks();
	void * res = gblState.malloc(size);
	t = getticks() - t;

	//profile
	MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->onMalloc(res,size,t,MALLOC_KIND_MALLOC));

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
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT;

	//profile
	MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->onFree(ptr,0));

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
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT

	//calloc need a special trick for first use due to usage in dlsym
	//this way it avoid to create infinite loop
	if (gblState.status == ALLOC_WRAP_INIT_SYM)
	{
		assert(sizeof(gblCallocIniBuffer) >= size);
		return gblCallocIniBuffer;
	}
	
	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	
	ticks t = getticks();
	void * res = gblState.calloc(nmemb,size);
	t = getticks() - t;

	//profile
	MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->onCalloc(res,nmemb,size,t));

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
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	
	ticks t = getticks();
	void * res = gblState.realloc(ptr,size);
	t = getticks() - t;
	
	//profile
	MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->onRealloc(ptr,res,size,t));
	
	return res;
}

/*******************  FUNCTION  *********************/
/**
 * Wrapper of the posix_memalign function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
int posix_memalign(void ** memptr,size_t align, size_t size)
{
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	
	ticks t = getticks();
	int res = gblState.posix_memalign(memptr,align,size);
	t = getticks() - t;

	//profile
	MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->onMalloc(*memptr,size,t,MALLOC_KIND_POSIX_MEMALIGN));

	//return segment to user
	return res;
}

/*******************  FUNCTION  *********************/
/**
 * Wrapper of the aligned_alloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
void * aligned_alloc(size_t alignment, size_t size)
{
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	
	ticks t = getticks();
	void * res = gblState.aligned_alloc(alignment,size);
	t = getticks() - t;

	//profile
	MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->onMalloc(res,size,t,MALLOC_KIND_ALIGNED_ALLOC));

	//return segment to user
	return res;
}

/*******************  FUNCTION  *********************/
/**
 * Wrapper of the memalign function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
void *memalign(size_t alignment, size_t size)
{
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	
	ticks t = getticks();
	void * res = gblState.memalign(alignment,size);
	t = getticks() - t;

	//profile
	MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->onMalloc(res,size,t,MALLOC_KIND_POSIX_MEMALIGN));

	//return segment to user
	return res;
}

/*******************  FUNCTION  *********************/
/**
 * Wrapper of the valloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
void *valloc(size_t size)
{
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	
	ticks t = getticks();
	void * res = gblState.valloc(size);
	t = getticks() - t;

	//profile
	MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->onMalloc(res,size,t,MALLOC_KIND_VALLOC));

	//return segment to user
	return res;
}

/*******************  FUNCTION  *********************/
/**
 * Wrapper of the pvalloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
void *pvalloc(size_t size)
{
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	
	ticks t = getticks();
	void * res = gblState.pvalloc(size);
	t = getticks() - t;

	//profile
	MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->onMalloc(res,size,t,MALLOC_KIND_PVALLOC));

	//return segment to user
	return res;
}

/*******************  FUNCTION  *********************/
/**
 * Wrapper of the pvalloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
void *mmap(void *start, size_t length, int prot,int flags,int fd, off_t offset)
{
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	void * res = gblState.mmap(start,length,prot,flags,fd,offset);

	//profile
	//MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->onMmap(res,length,flags,fd));

	//return segment to user
	return res;
}

/*******************  FUNCTION  *********************/
/**
 * Wrapper of the pvalloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
int munmap(void *start, size_t length)
{
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	int res = gblState.munmap(start,length);

	//profile
	//MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->onMalloc(res,size));

	//return segment to user
	return res;
}

/*******************  FUNCTION  *********************/
/**
 * Wrapper of the pvalloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
int mremap(void *old_address, size_t old_size , size_t new_size, int flags)
{
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	int res = gblState.mremap(old_address,old_size,new_size,flags);

	//profile
	//MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->onMalloc(res,size));

	//return segment to user
	return res;
}

/*********************  STRUCT  *********************/
//Juste to be sure of the C symbol naming.
extern "C" 
{
	void __cyg_profile_func_enter (void *this_fn,void *call_site);
	void __cyg_profile_func_exit  (void *this_fn,void *call_site);
}

/*******************  FUNCTION  *********************/
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
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT
	
	//stack tracking
	if (tlsState.status == ALLOC_WRAP_READY || tlsState.status == ALLOC_WRAP_DISABLED)
		localState.profiler->onEnterFunc(this_fn,call_site);
}

/*******************  FUNCTION  *********************/
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
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT
	
	//stack tracking
	if (tlsState.status == ALLOC_WRAP_READY || tlsState.status == ALLOC_WRAP_DISABLED)
		localState.profiler->onExitFunc(this_fn,call_site);
}


/*******************  FUNCTION  *********************/
extern "C" {
	void maqao_enter_function(int fid,const char * funcName);
	void maqao_exit_function(int fid,const char * funcName);
	void maqao_reg_function(int funcId,const char * funcName,const char * file,int line);
}

/*******************  FUNCTION  *********************/
void maqao_enter_function(int fid,const char * funcName)
{
	__cyg_profile_func_enter((void*)(size_t)fid,(void*)(size_t)fid);
}

/*******************  FUNCTION  *********************/
void maqao_exit_function(int fid,const char * funcName)
{
	__cyg_profile_func_exit((void*)(size_t)fid,(void*)(size_t)fid);
}

/*******************  FUNCTION  *********************/
void maqao_reg_function(int funcId,const char * funcName,const char * file,int line)
{
	//check init
	if (gblState.status == ALLOC_WRAP_NOT_READY)
		free(NULL);

	//mark stack mode to be automatic
	gblState.options->stackMode = STACK_MODE_ENTER_EXIT_FUNC;
	
	//reg
	gblState.profiler->registerMaqaoFunctionSymbol(funcId,funcName,file,line);
}

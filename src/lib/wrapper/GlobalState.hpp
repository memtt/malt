/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : ./src/lib/wrapper/GlobalState.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
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
//internals
#include <common/Debug.hpp>
#include <portability/OS.hpp>
#include <portability/Mutex.hpp>
#include <common/CodeTiming.hpp>
#include <core/StackSizeTracker.hpp>
#include <profiler/AllocStackProfiler.hpp>
#include <profiler/LocalAllocStackProfiler.hpp>
//locals
#include "malt.h"
#include "AllocWrapperExtend.hpp"

/**********************************************************/
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

/**********************************************************/
/** Check init status of local and global state and call enter/exit methods, then do requested action. **/
#define MALT_WRAPPER_LOCAL_STATE_ACTION(action, retAddr)  \
	if (gblState.status == ALLOC_WRAP_READY && tlsState.status == ALLOC_WRAP_READY) \
	{ \
		if (isEnterExit)\
		{\
			retAddr =__builtin_extract_return_addr(__builtin_return_address(0)); \
			localState.profiler->onEnterFunc((void*)__func__,retAddr,true); \
		}\
		do{action;}while(0); \
		if (isEnterExit) \
			localState.profiler->onExitFunc((void*)__func__,retAddr,true); \
	}


/**********************************************************/
namespace MALT
{

/**********************************************************/
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

/**********************************************************/
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
	MallocFuncPtr malloc;
	/** Pointer to the old (glibc) free symbol. **/
	FreeFuncPtr free;
	/** Pointer to the old (glibc) calloc symbol. **/
	CallocFuncPtr calloc;
	/** Pointer to the old (glibc) realloc symbol. **/
	ReallocFuncPtr realloc;
	/** Pointer to the old (libc) posix_memalign symbol. **/
	PosixMemalignFuncPtr posix_memalign;
	/** Pointer to the old (libc) aligned_alloc symbol. **/
	AlignedAllocFuncPtr aligned_alloc;
	/** Pointer to the old (libc) valloc symbol. **/
	VallocFuncPtr valloc;
	/** Pointer to the old (libc) pvalloc symbol. **/
	PVallocFuncPtr pvalloc;
	/** Pointer to the old (libc) memalign symbol. **/
	MemalignFuncPtr memalign;
	/** Pointer to the old (libc) mmap symbol. **/
	MmapFuncPtr mmap;
	/** Pointer to the old (libc) munmap symbol. **/
	MunmapFuncPtr munmap;
	/** Pointer to the old (libc) mremap symbol. **/
	MremapFuncPtr mremap;
	/** Pointer to the profiler (use pointer due to use before main, only way to ensure init before first malloc usage). **/
	AllocStackProfiler * profiler;
	/** Keep track of user options. **/
	Options * options;
	/** Function used to init the structure on first use. **/
	void init(void);
	/** Function to cleanup the structure at exit. **/
	static void onExit(void);
};

/**********************************************************/
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

/**********************************************************/
/** Store the global state of allocator wrapper. **/
extern AllocWrapperGlobal gblState;
/** Store the per-thread state of allocator wrapper. **/
extern __thread ThreadLocalState tlsState;
/** Temporary buffer to return on first realloc used by dlsym and split infinit call loops. **/
extern char gblCallocIniBuffer[4096];

}

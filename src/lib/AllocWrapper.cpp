/********************  HEADERS  *********************/
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <dlfcn.h>
#include "AllocStackProfiler.h"

/*******************  FUNCTION  *********************/
typedef void * (*MallocFuncPtr) (size_t size);
typedef void   (*FreeFuncPtr)   (void * ptr);
typedef void * (*CallocFuncPtr) (size_t nmemb,size_t size);
typedef void * (*ReallocFuncPtr)(void * ptr,size_t size);

/********************  ENUM  ************************/
enum RealAllocatorState
{
	REAL_ALLOC_NOT_READY,
	REAL_ALLOC_INIT_SYM,
	REAL_ALLOC_INIT_PROFILER,
	REAL_ALLOC_READY,
	REAL_ALLOC_INUSE
};

/*********************  STRUCT  *********************/
struct RealAllocator
{
	//init state
	RealAllocatorState state;
	//real allocator functions
	MallocFuncPtr malloc;
	FreeFuncPtr free;
	CallocFuncPtr calloc;
	ReallocFuncPtr realloc;
	//profiler
	AllocStackProfiler profiler;
	//init function
	void init(void);
	static void onExit(int status,void * arg);
};

/********************  GLOBALS  **********************/
static RealAllocator gblRealAlloc = {REAL_ALLOC_NOT_READY};
static char gblCallocIniBuffer[4096];

/*******************  FUNCTION  *********************/
//TODO need lock
void RealAllocator::init(void )
{
	if (gblRealAlloc.state == REAL_ALLOC_NOT_READY)
	{
		//update state
		gblRealAlloc.state = REAL_ALLOC_INIT_SYM;
		
		//search addresses
		gblRealAlloc.malloc = (MallocFuncPtr)dlsym(RTLD_NEXT,"malloc");
		gblRealAlloc.free = (FreeFuncPtr)dlsym(RTLD_NEXT,"free");
		gblRealAlloc.calloc = (CallocFuncPtr)dlsym(RTLD_NEXT,"calloc");
		gblRealAlloc.realloc = (ReallocFuncPtr)dlsym(RTLD_NEXT,"realloc");

		//init profiler
		gblRealAlloc.state = REAL_ALLOC_INIT_PROFILER;
		gblRealAlloc.profiler.init();

		//register on exit
		on_exit(RealAllocator::onExit,NULL);

		//final state
		gblRealAlloc.state = REAL_ALLOC_READY;
	}
}

/*******************  FUNCTION  *********************/
void RealAllocator::onExit(int status,void * arg)
{
	gblRealAlloc.profiler.onExit();
}

/*******************  FUNCTION  *********************/
void * malloc(size_t size)
{
	//check init
	if (gblRealAlloc.state == REAL_ALLOC_NOT_READY)
		gblRealAlloc.init();

	//run the default function
	assert(gblRealAlloc.state > REAL_ALLOC_INIT_SYM);
	void * res = gblRealAlloc.malloc(size);

	//profile
	if (gblRealAlloc.state == REAL_ALLOC_READY)
	{
		gblRealAlloc.state = REAL_ALLOC_INUSE;
		gblRealAlloc.profiler.onMalloc(res,size);
		gblRealAlloc.state = REAL_ALLOC_READY;
	}

	//return segment to user
	return res;
}

/*******************  FUNCTION  *********************/
void free(void * ptr)
{
	//check init
	if (gblRealAlloc.state == REAL_ALLOC_NOT_READY)
		gblRealAlloc.init();

	//profile
	if (gblRealAlloc.state == REAL_ALLOC_READY)
	{
		gblRealAlloc.state = REAL_ALLOC_INUSE;
		gblRealAlloc.profiler.onFree(ptr);
		gblRealAlloc.state = REAL_ALLOC_READY;
	}

	//run the default function
	assert(gblRealAlloc.state > REAL_ALLOC_INIT_SYM);
	gblRealAlloc.free(ptr);
}

/*******************  FUNCTION  *********************/
void * calloc(size_t nmemb,size_t size)
{
	//check init
	if (gblRealAlloc.state == REAL_ALLOC_NOT_READY)
		gblRealAlloc.init();

	//calloc need a special trick for first use due to usage in dlsym
	//this way it avoid to create infinite loop
	if (gblRealAlloc.state == REAL_ALLOC_INIT_SYM)
	{
		assert(sizeof(gblCallocIniBuffer) >= size);
		return gblCallocIniBuffer;
	}
	
	//run the default function
	assert(gblRealAlloc.state > REAL_ALLOC_INIT_SYM);
	void * res = gblRealAlloc.calloc(nmemb,size);

	//profile
	if (gblRealAlloc.state == REAL_ALLOC_READY)
	{
		gblRealAlloc.state = REAL_ALLOC_INUSE;
		gblRealAlloc.profiler.onCalloc(res,nmemb,size);
		gblRealAlloc.state = REAL_ALLOC_READY;
	}

	//return result to user
	return res;
}

/*******************  FUNCTION  *********************/
void * realloc(void * ptr, size_t size)
{
	//check init
	if (gblRealAlloc.state == REAL_ALLOC_NOT_READY)
		gblRealAlloc.init();

	//run the default function
	assert(gblRealAlloc.state > REAL_ALLOC_INIT_SYM);
	return gblRealAlloc.realloc(ptr,size);
}

/********************  HEADERS  *********************/
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <dlfcn.h>
#include <cstring>
#include <portability/Mutex.h>
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
	REAL_ALLOC_INUSE,
	REAL_ALLOC_FINISH,
};

/*********************  STRUCT  *********************/
struct GlobalState
{
	//init state CAUTION, MUST LET state,lock in same order at first due to static pre-initialization.
	RealAllocatorState state;
	StaticMutex lock;
	//real allocator functions
	MallocFuncPtr malloc;
	FreeFuncPtr free;
	CallocFuncPtr calloc;
	ReallocFuncPtr realloc;
	//profiler
	AllocStackProfiler * profiler;
	//init function
	void init(void);
	static void onExit(int status,void * arg);
};

/********************  STRUCT  **********************/
struct ThreadLocalState
{
	EnterExitCallStack * enterExitStack;
	bool inUse;
	bool init;
};

/********************  GLOBALS  **********************/
static GlobalState gblState = {REAL_ALLOC_NOT_READY,STATIC_MUTEX_INIT,NULL,NULL,NULL,NULL};
static char gblCallocIniBuffer[4096];
static __thread ThreadLocalState tlsState = {NULL,false,false};

/*******************  FUNCTION  *********************/
static StackMode getStackMode(void)
{
	char * mode = getenv("STACK_MODE");
	if (mode == NULL)
	{
		return STACK_MODE_BACKTRACE;
	} else if (strcmp(mode,"backtrace") == 0 || strcmp(mode,"") == 0) {
		return STACK_MODE_BACKTRACE;
	} else if (strcmp(mode,"enter-exit") == 0) {
		return STACK_MODE_USER;
	} else {
		fprintf(stderr,"Invalid mode in STACK_MODE environnement variable : '%s'! Supportted : backtrace | enter-exit.",mode);
		abort();
	}
}

/*******************  FUNCTION  *********************/
//TODO need lock
void GlobalState::init(void )
{
	//secure in case of first call in threads
	gblState.lock.lock();

	if (gblState.state == REAL_ALLOC_NOT_READY)
	{
		//update state
		gblState.state = REAL_ALLOC_INIT_SYM;
		
		//search addresses
		gblState.malloc = (MallocFuncPtr)dlsym(RTLD_NEXT,"malloc");
		gblState.free = (FreeFuncPtr)dlsym(RTLD_NEXT,"free");
		gblState.calloc = (CallocFuncPtr)dlsym(RTLD_NEXT,"calloc");
		gblState.realloc = (ReallocFuncPtr)dlsym(RTLD_NEXT,"realloc");

		//init profiler
		gblState.state = REAL_ALLOC_INIT_PROFILER;
		gblState.profiler = new AllocStackProfiler(getStackMode());

		//register on exit
		on_exit(GlobalState::onExit,NULL);

		//final state
		gblState.state = REAL_ALLOC_READY;
	}
	
	//secure in case of first call in threads
	gblState.lock.unlock();
}

/*******************  FUNCTION  *********************/
void GlobalState::onExit(int status,void * arg)
{
	if (gblState.state == REAL_ALLOC_READY)
	{
		gblState.profiler->onExit();
		gblState.state = REAL_ALLOC_FINISH;
		delete gblState.profiler;
	} else {
		gblState.state = REAL_ALLOC_FINISH;
	}
}

/*******************  FUNCTION  *********************/
static EnterExitCallStack * initTls(void)
{
	assert(tlsState.enterExitStack == NULL);
	//check init
	if (gblState.state == REAL_ALLOC_NOT_READY)
		gblState.init();
	
	//create the local chain
	EnterExitCallStack * res = new EnterExitCallStack();
	res->enterFunction((void*)0x1);
	
	//update TLS
	tlsState.enterExitStack = res;
	tlsState.init = true;
	
	return res;
}

/*******************  FUNCTION  *********************/
void * malloc(size_t size)
{
	//get addr localy to avoid to read the TLS every time
	ThreadLocalState & localState = tlsState;
	
	//check init
	if ( ! localState.init )
		initTls();

	//run the default function
	assert(gblState.state > REAL_ALLOC_INIT_SYM);
	void * res = gblState.malloc(size);

	//profile
	if (!localState.inUse && gblState.state == REAL_ALLOC_READY)
	{
		localState.inUse = true;
		gblState.profiler->onMalloc(res,size,localState.enterExitStack);
		localState.inUse = false;
	}

	//return segment to user
	return res;
}

/*******************  FUNCTION  *********************/
void free(void * ptr)
{
	//get addr localy to avoid to read the TLS every time
	ThreadLocalState & localState = tlsState;
	
	//check init
	if ( ! localState.init )
		initTls();

	//profile
	if (!localState.inUse && gblState.state == REAL_ALLOC_READY)
	{
		localState.inUse = true;
		gblState.profiler->onFree(ptr,localState.enterExitStack);
		localState.inUse = false;
	}

	//run the default function
	assert(gblState.state > REAL_ALLOC_INIT_SYM);
	gblState.free(ptr);
}

/*******************  FUNCTION  *********************/
void * calloc(size_t nmemb,size_t size)
{
	//get addr localy to avoid to read the TLS every time
	ThreadLocalState & localState = tlsState;
	
	//check init
	if ( ! localState.init )
		initTls();

	//calloc need a special trick for first use due to usage in dlsym
	//this way it avoid to create infinite loop
	if (gblState.state == REAL_ALLOC_INIT_SYM)
	{
		assert(sizeof(gblCallocIniBuffer) >= size);
		return gblCallocIniBuffer;
	}
	
	//run the default function
	assert(gblState.state > REAL_ALLOC_INIT_SYM);
	void * res = gblState.calloc(nmemb,size);

	//profile
	if (!localState.inUse && gblState.state == REAL_ALLOC_READY)
	{
		localState.inUse = true;
		gblState.profiler->onCalloc(res,nmemb,size,localState.enterExitStack);
		localState.inUse = false;
	}

	//return result to user
	return res;
}

/*******************  FUNCTION  *********************/
void * realloc(void * ptr, size_t size)
{
	//get addr localy to avoid to read the TLS every time
	ThreadLocalState & localState = tlsState;
	
	//check init
	if ( ! localState.init )
		initTls();

	//run the default function
	assert(gblState.state > REAL_ALLOC_INIT_SYM);
	return gblState.realloc(ptr,size);
}

/*********************  STRUCT  *********************/
extern "C" 
{
	void __cyg_profile_func_enter (void *this_fn,void *call_site);
	void __cyg_profile_func_exit  (void *this_fn,void *call_site);
}

/*********************  STRUCT  *********************/
void __cyg_profile_func_enter (void *this_fn,void *call_site)
{
	//get the local stack
	EnterExitCallStack * stack = tlsState.enterExitStack;
	if (stack == NULL)
		stack = initTls();
	
	stack->enterFunction(this_fn);
}

/*********************  STRUCT  *********************/
void __cyg_profile_func_exit  (void *this_fn,void *call_site)
{
	//get the local stack
	EnterExitCallStack * stack = tlsState.enterExitStack;
	if (stack == NULL)
		stack = initTls();
	
	stack->exitFunction(this_fn);
}

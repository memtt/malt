/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 06/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/wrappers/WrapperCAlloc.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014 - 2022
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include "state/LazyEnv.hpp"
#include "state/ReentranceGuard.hpp"
#include "WrapperCAlloc.hpp"
#include "common/Helpers.hpp"
#include "injectors/InjectPythonInit.hpp"
#include <sys/mman.h>

/**********************************************************/
using namespace MALT;

/**********************************************************/
namespace MALT {
	static std::list<std::string> * gblMaltWrappedSymbols = nullptr;
	static const bool cstInstrMmap = true;
}

/**********************************************************/
/** Check init status of local and global state and call enter/exit methods, then do requested action. **/
#define MALT_WRAPPER_LOCAL_STATE_ACTION(action, retAddr)  \
	do \
	{ \
		if (env.isEnterExit())\
		{\
			retAddr =__builtin_extract_return_addr(__builtin_return_address(0)); \
			env.getLocalProfiler().onEnterFunc(LangAddress(DOMAIN_C, (void*)__func__),LangAddress(DOMAIN_C, retAddr),true); \
		}\
		do{action;}while(0); \
		if (env.isEnterExit()) \
		env.getLocalProfiler().onExitFunc(LangAddress(DOMAIN_C, (void*)__func__),LangAddress(DOMAIN_C, retAddr),true); \
	} while(0)

/**********************************************************/
void * MALT::malt_wrap_malloc(size_t size, const MallocFuncPtr & real_malloc, void * retaddr)
{
	//get local TLS and check init
	LazyEnv env;
	ReentranceGuard guard(env);

	//run the default function
	assert(env.getGlobalState().status > ALLOC_WRAP_INIT_SYM);
	
	ticks t = Clock::getticks();
	void * res = real_malloc(size);
	t = Clock::getticks() - t;

	//profile
	if (guard.needInstrument()) {
		//lazily init python when we see it init
		//This is required for Ubuntu & Debian Python because we cannot hook in
		//Py_RunMain() as I tried first.
		initPythonLazy();
		initMpiRankFilter();

		//instru
		MALT_WRAPPER_LOCAL_STATE_ACTION(env.getLocalProfiler().onMalloc(res,size,t,MALLOC_KIND_MALLOC, LANG_C, DOMAIN_C_ALLOC), retaddr);
	}

	//return segment to user
	return res;
}

/**********************************************************/
void MALT::malt_wrap_free(void * ptr, const FreeFuncPtr & real_free, void * retaddr)
{
	//need to ignore them otherwise enter sometime in an infinite call loop
	//when using numpy with python
	if (ptr == NULL || ptr == gblCallocIniBuffer)
		return;

	//get local TLS and check init
	LazyEnv env;
	ReentranceGuard guard(env);

	//profile
	if (guard.needInstrument()) {
		MALT_WRAPPER_LOCAL_STATE_ACTION(env.getLocalProfiler().onFree(ptr,0), retaddr);
	}

	//run the default function
	//assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	if (real_free != nullptr)
		real_free(ptr);
}

/**********************************************************/
void * MALT::malt_wrap_calloc(size_t nmemb,size_t size, const CallocFuncPtr & real_calloc, void * retaddr)
{
	//get local TLS and check init
	LazyEnv env;
	ReentranceGuard guard(env);

	//calloc need a special trick for first use due to usage in dlsym
	//this way it avoid to create infinite loop
	if (gblState.status == ALLOC_WRAP_INIT_SYM)
	{
		assert(sizeof(gblCallocIniBuffer) >= size);
		return gblCallocIniBuffer;
	}
	
	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	
	ticks t = Clock::getticks();
	void * res = real_calloc(nmemb,size);
	t = Clock::getticks() - t;

	//profile
	if (guard.needInstrument()) {
		MALT_WRAPPER_LOCAL_STATE_ACTION(env.getLocalProfiler().onCalloc(res,nmemb,size,t, LANG_C, DOMAIN_C_ALLOC), retaddr);
	}

	//return result to user
	return res;
}

/**********************************************************/
void * MALT::malt_wrap_realloc(void * ptr,size_t size, const ReallocFuncPtr & real_realloc, void * retaddr)
{
	//get local TLS and check init
	LazyEnv env;
	ReentranceGuard guard(env);

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	
	ticks t = Clock::getticks();
	void * res = real_realloc(ptr,size);
	t = Clock::getticks() - t;
	
	//profile
	if (guard.needInstrument()) {
		MALT_WRAPPER_LOCAL_STATE_ACTION(env.getLocalProfiler().onRealloc(ptr,res,size,t, LANG_C, DOMAIN_C_ALLOC), retaddr);
	}
	
	return res;
}

/**********************************************************/
int MALT::malt_wrap_posix_memalign(void **memptr, size_t alignment, size_t size, const PosixMemalignFuncPtr & real_mem_align, void * retaddr)
{
	//get local TLS and check init
	LazyEnv env;
	ReentranceGuard guard(env);

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	
	ticks t = Clock::getticks();
	int res = real_mem_align(memptr, alignment, size);
	t = Clock::getticks() - t;

	//profile
	if (guard.needInstrument()) {
		MALT_WRAPPER_LOCAL_STATE_ACTION(env.getLocalProfiler().onMalloc(*memptr,size,t,MALLOC_KIND_POSIX_MEMALIGN, LANG_C, DOMAIN_C_ALLOC), retaddr);
	}

	//return segment to user
	return res;
}

/**********************************************************/
void * MALT::malt_wrap_aligned_alloc(size_t alignment, size_t size, const AlignedAllocFuncPtr & real_aligned_alloc, void * retaddr)
{
	//get local TLS and check init
	LazyEnv env;
	ReentranceGuard guard(env);

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	
	ticks t = Clock::getticks();
	void * res = real_aligned_alloc(alignment,size);
	t = Clock::getticks() - t;

	//profile
	if (guard.needInstrument()) {
		MALT_WRAPPER_LOCAL_STATE_ACTION(env.getLocalProfiler().onMalloc(res,size,t,MALLOC_KIND_ALIGNED_ALLOC, LANG_C, DOMAIN_C_ALLOC), retaddr);
	}

	//return segment to user
	return res;
}

/**********************************************************/
void *MALT::malt_wrap_memalign(size_t alignment, size_t size, const MemalignFuncPtr & real_memalign, void * retaddr)
{
	//get local TLS and check init
	LazyEnv env;
	ReentranceGuard guard(env);

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	
	ticks t = Clock::getticks();
	void * res = real_memalign(alignment,size);
	t = Clock::getticks() - t;

	//profile
	if (guard.needInstrument()) {
		MALT_WRAPPER_LOCAL_STATE_ACTION(env.getLocalProfiler().onMalloc(res,size,t,MALLOC_KIND_POSIX_MEMALIGN, LANG_C, DOMAIN_C_ALLOC), retaddr);
	}

	//return segment to user
	return res;
}

/**********************************************************/
void * MALT::malt_wrap_valloc(size_t size, const VallocFuncPtr & real_aligned_valloc, void * retaddr)
{
	//get local TLS and check init
	LazyEnv env;
	ReentranceGuard guard(env);

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	
	ticks t = Clock::getticks();
	void * res = real_aligned_valloc(size);
	t = Clock::getticks() - t;

	//profile
	if (guard.needInstrument()) {
		MALT_WRAPPER_LOCAL_STATE_ACTION(env.getLocalProfiler().onMalloc(res,size,t,MALLOC_KIND_VALLOC, LANG_C, DOMAIN_C_ALLOC), retaddr);
	}

	//return segment to user
	return res;
}

/**********************************************************/
void *MALT::malt_wrap_pvalloc(size_t size, const PVallocFuncPtr & real_pvalloc, void * retaddr)
{
	//get local TLS and check init
	LazyEnv env;
	ReentranceGuard guard(env);

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	
	ticks t = Clock::getticks();
	void * res = real_pvalloc(size);
	t = Clock::getticks() - t;

	//profile
	if (guard.needInstrument()) {
		MALT_WRAPPER_LOCAL_STATE_ACTION(env.getLocalProfiler().onMalloc(res,size,t,MALLOC_KIND_PVALLOC, LANG_C, DOMAIN_C_ALLOC), retaddr);
	}

	//return segment to user
	return res;
}

/**********************************************************/
void * MALT::malt_wrap_mmap(void *start,size_t length,int prot,int flags,int fd,off_t offset, const MmapFuncPtr & real_mmap, void * retaddr)
{
	//get local TLS and check init
	LazyEnv env;
	ReentranceGuard guard(env);

	//if in internal je malloc
	if (env.isInInternalJeMalloc()) {
		env.getGlobalProfiler().registerMaltJeMallocMem(length);
		flags += MAP_POPULATE;
	}

	//check
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);

	//run
	ticks t = Clock::getticks();
	void * res = real_mmap(start,length,prot,flags,fd,offset);
	t = Clock::getticks() - t;

	//profile
	if (guard.needInstrument() && res != MAP_FAILED && cstInstrMmap) {
		MALT_WRAPPER_LOCAL_STATE_ACTION(env.getLocalProfiler().onMmap(res,length,flags,fd,t), retaddr);
	}

	//return segment to user
	return res;
}

/**********************************************************/
int MALT::malt_wrap_munmap(void * start, size_t length, const MunmapFuncPtr & real_munmap, void * retaddr)
{
	//get local TLS and check init
	LazyEnv env;
	ReentranceGuard guard(env);

	//if in internal je malloc
	if (env.isInInternalJeMalloc()) {
		env.getGlobalProfiler().registerMaltJeMallocMem(-length);
	}

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);

	ticks t = Clock::getticks();
	int res = real_munmap(start,length);
	t = Clock::getticks() - t;

	//profile
	if (guard.needInstrument() && res == 0) {
		MALT_WRAPPER_LOCAL_STATE_ACTION(env.getLocalProfiler().onMunmap(start,length, t), retaddr);
	}

	//return segment to user
	return res;
}

/**********************************************************/
void * MALT::malt_wrap_mremap(void *old_address, size_t old_size , void * new_address, size_t new_size, int flags, const MremapFuncPtr & real_mremap, void * retaddr)
{
	//get local TLS and check init
	LazyEnv env;
	ReentranceGuard guard(env);

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);

	ticks t = Clock::getticks();
	void * res = real_mremap(old_address,old_size,new_size,flags,new_address);
	t = Clock::getticks() - t;

	//profile
	if (guard.needInstrument() && res != MAP_FAILED && cstInstrMmap) {
		MALT_WRAPPER_LOCAL_STATE_ACTION(env.getLocalProfiler().onMremap(old_address,old_size, res, new_size, t), retaddr);
	}

	//return segment to user
	return res;
}

/**********************************************************/
void MALT::maltRegisterWrappedSymbol(const char * wrapped_symbol)
{
	if (gblMaltWrappedSymbols == nullptr)
		gblMaltWrappedSymbols = new std::list<std::string>();
	gblMaltWrappedSymbols->push_back(wrapped_symbol);
}

/**********************************************************/
const std::list<std::string> & MALT::maltGetWrappedSymbols(void)
{
	if (gblMaltWrappedSymbols == nullptr)
		gblMaltWrappedSymbols = new std::list<std::string>();
	return *gblMaltWrappedSymbols;
}

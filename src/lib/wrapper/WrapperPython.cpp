/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : ./src/lib/wrapper/InjectCAlloc.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <Python.h>
#include "GlobalState.hpp"
#include "WrapperPython.hpp"
#include <stacks/BacktracePythonStack.hpp>

/**********************************************************/
namespace MALT
{

/**********************************************************/
void * malt_wrap_python_malloc(void * ctx, size_t size, PythonMallocFuncPtr real_malloc, void * retaddr)
{
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	
	//not to reenter in low level alloc
	bool oldStatus;
	MALT_WRAPPER_LOCAL_STATE_ACTION(oldStatus = localState.profiler->markInUseAndGetOldStatus(), retaddr);

	//real call
	ticks t = Clock::getticks();
	void * res = real_malloc(ctx, size);
	t = Clock::getticks() - t;

	//end of not reenter
	MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->restoreInUseStatus(oldStatus), retaddr);

	//profile
	MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->onMalloc(res,size,t,MALLOC_KIND_MALLOC,LANG_PYTHON), retaddr);

	//return segment to user
	return res;
}

/**********************************************************/
void malt_wrap_python_free(void * ctx, void * ptr, PythonFreeFuncPtr real_free, void * retaddr)
{
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT;

	//profile
	MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->onFree(ptr,0,LANG_PYTHON), retaddr);

	//not to reenter in low level alloc
	bool oldStatus;
	MALT_WRAPPER_LOCAL_STATE_ACTION(oldStatus = localState.profiler->markInUseAndGetOldStatus(), retaddr);

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	real_free(ctx, ptr);

	//end of not reenter
	MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->restoreInUseStatus(oldStatus), retaddr);
}

/**********************************************************/
void * malt_wrap_python_calloc(void * ctx, size_t nmemb,size_t size, PythonCallocFuncPtr real_calloc, void * retaddr)
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

	//not to reenter in low level alloc
	bool oldStatus;
	MALT_WRAPPER_LOCAL_STATE_ACTION(oldStatus = localState.profiler->markInUseAndGetOldStatus(), retaddr);
	
	ticks t = Clock::getticks();
	void * res = real_calloc(ctx,nmemb,size);
	t = Clock::getticks() - t;

	//end of not reenter
	MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->restoreInUseStatus(oldStatus), retaddr);

	//profile
	MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->onCalloc(res,nmemb,size,t,LANG_PYTHON), retaddr);

	//return result to user
	return res;
}

/**********************************************************/
void * malt_wrap_python_realloc(void * ctx,void * ptr,size_t size, PythonReallocFuncPtr real_realloc, void * retaddr)
{
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);

	//not to reenter in low level alloc
	bool oldStatus;
	MALT_WRAPPER_LOCAL_STATE_ACTION(oldStatus = localState.profiler->markInUseAndGetOldStatus(), retaddr);
	
	ticks t = Clock::getticks();
	void * res = real_realloc(ctx,ptr,size);
	t = Clock::getticks() - t;

	//end of not reenter
	MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->restoreInUseStatus(oldStatus), retaddr);
	
	//profile
	MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->onRealloc(ptr,res,size,t,LANG_PYTHON), retaddr);
	
	return res;
}

/**********************************************************/
int malt_wrap_python_on_enter_exit(PyObject *obj, PyFrameObject *frame, int what, PyObject *arg)
{
	MALT::PythonSymbolTracker & tracker = gblState.profiler->getPythonSymbolTracker();
	LangAddress addr = tracker.frameToLangAddress(frame);
	MALT::PythonCallSite site = tracker.getCallSite(addr);
	switch (what)
	{
		case PyTrace_CALL:
			printf("Enter call %p %s\n", frame, site.function);
			break;
		case PyTrace_RETURN:
			printf("Return call %p %s\n", frame, site.function);
			break;
		case PyTrace_C_CALL:
			printf("Enter C call %p %s\n", frame, site.function);
			break;
		case PyTrace_C_RETURN:
			printf("Edit C call %p %s\n", frame, site.function);
			break;
		default:{}
			//ignored
	};

	//ok
	return 0;
}

}

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
/** Check init status of local and global state and call enter/exit methods, then do requested action. **/
#define MALT_WRAPPER_LOCAL_STATE_ACTION_PYTHON(action)  \
	if (gblState.status == ALLOC_WRAP_READY && tlsState.status == ALLOC_WRAP_READY) \
	{ \
		LangAddress __retAddr__ = nullAddr; \
		if (localState.profiler->isInUse() == false && localState.profiler->isEnterExit()) { \
			bool __oldStatus = localState.profiler->markInUseAndGetOldStatus(); \
			__retAddr__ = localState.profiler->getBacktracePythonStack().getCurrentFrameAddr(); \
			localState.profiler->restoreInUseStatus(__oldStatus); \
		} \
		if (isEnterExit)\
		{\
			localState.profiler->onEnterFunc(nullAddr,__retAddr__,true); \
		}\
		do{action;}while(0); \
		if (isEnterExit) \
			localState.profiler->onExitFunc(nullAddr,__retAddr__,true); \
	}

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
	oldStatus = localState.profiler->markInUseAndGetOldStatus();

	//real call
	ticks t = Clock::getticks();
	void * res = real_malloc(ctx, size);
	t = Clock::getticks() - t;

	//end of not reenter
	localState.profiler->restoreInUseStatus(oldStatus);

	//profile
	MALT_WRAPPER_LOCAL_STATE_ACTION_PYTHON(localState.profiler->onMalloc(res,size,t,MALLOC_KIND_MALLOC,LANG_PYTHON));

	//return segment to user
	return res;
}

/**********************************************************/
void malt_wrap_python_free(void * ctx, void * ptr, PythonFreeFuncPtr real_free, void * retaddr)
{
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT;

	//profile
	MALT_WRAPPER_LOCAL_STATE_ACTION_PYTHON(localState.profiler->onFree(ptr,0,LANG_PYTHON));

	//not to reenter in low level alloc
	bool oldStatus = localState.profiler->markInUseAndGetOldStatus();

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	real_free(ctx, ptr);

	//end of not reenter
	localState.profiler->restoreInUseStatus(oldStatus);
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
	bool oldStatus = localState.profiler->markInUseAndGetOldStatus();
	
	ticks t = Clock::getticks();
	void * res = real_calloc(ctx,nmemb,size);
	t = Clock::getticks() - t;

	//end of not reenter
	localState.profiler->restoreInUseStatus(oldStatus);

	//profile
	MALT_WRAPPER_LOCAL_STATE_ACTION_PYTHON(localState.profiler->onCalloc(res,nmemb,size,t,LANG_PYTHON));

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
	bool oldStatus = localState.profiler->markInUseAndGetOldStatus();
	
	ticks t = Clock::getticks();
	void * res = real_realloc(ctx,ptr,size);
	t = Clock::getticks() - t;

	//end of not reenter
	localState.profiler->restoreInUseStatus(oldStatus);
	
	//profile
	MALT_WRAPPER_LOCAL_STATE_ACTION_PYTHON(localState.profiler->onRealloc(ptr,res,size,t,LANG_PYTHON));
	
	return res;
}

/**********************************************************/
int malt_wrap_python_on_enter_exit(PyObject *obj, PyFrameObject *frame, int what, PyObject *arg)
{
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT

	//not to reenter in low level alloc
	bool oldStatus;
	oldStatus = localState.profiler->markInUseAndGetOldStatus();

	//apply
	switch (what)
	{
		case PyTrace_CALL: {
			MALT::PythonSymbolTracker & tracker = gblState.profiler->getPythonSymbolTracker();
			LangAddress parentAddr = tracker.parentFrameToLangAddress(frame);
			//get up
			PythonCallSite site = tracker.getCallSite(parentAddr);
			localState.profiler->onEnterFunc(nullAddr,parentAddr,true);
			printf("enter in %s:%s:%d\n", site.file, site.function, site.line);
			//////////////////////
			/*PyFrameObject * parentFrame = PyFrame_GetBack(frame);
			if (parentFrame != nullptr) {
				localState.profiler->onEnterFunc(nullAddr,LangAddress(DOMAIN_PYTHON_FRAME, parentFrame),true);
				//Py_DECREF(parentFrame);
			}*/

			break;
		}
		case PyTrace_RETURN: {
			MALT::PythonSymbolTracker & tracker = gblState.profiler->getPythonSymbolTracker();
			LangAddress parentAddr = tracker.parentFrameToLangAddress(frame);
			PythonCallSite site = tracker.getCallSite(parentAddr);
			localState.profiler->onExitFunc(nullAddr,parentAddr,true);
			printf("exit in %s:%s:%d\n", site.file, site.function, site.line);
			//////////////
			/*PyFrameObject * parentFrame = PyFrame_GetBack(frame);
			if (parentFrame != nullptr) {
				localState.profiler->onExitFunc(nullAddr,LangAddress(DOMAIN_PYTHON_FRAME, parentFrame),true);
				//Py_DECREF(parentFrame);
			}*/
			break;
		}
		/*case PyTrace_C_CALL:
			localState.profiler->onEnterFunc(nullAddr, LangAddress(DOMAIN_PYTHON, MALT_PYTHON_C_BRIDGE_FUNC_ID),true);
			break;
		case PyTrace_C_RETURN:
			localState.profiler->onExitFunc(nullAddr, LangAddress(DOMAIN_PYTHON, MALT_PYTHON_C_BRIDGE_FUNC_ID),true);
			break;*/
		default:{
			//ignored
		}
	};

	//end of not reenter
	localState.profiler->restoreInUseStatus(oldStatus);

	//ok
	return 0;
}

bool malt_wrap_python_mark_in_use(void)
{
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT
	return localState.profiler->markInUseAndGetOldStatus();
}

void malt_wrap_python_restore_in_use(bool oldValue)
{
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT
	localState.profiler->restoreInUseStatus(oldValue);
}

}

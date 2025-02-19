/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/wrappers/WrapperPython.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <Python.h>
#include "state/LazyEnv.hpp"
#include "state/ReentranceGuard.hpp"
#include "WrapperPython.hpp"
#include <stacks/BacktracePythonStack.hpp>

/**********************************************************/
/** Check init status of local and global state and call enter/exit methods, then do requested action. **/
#define MALT_WRAPPER_LOCAL_STATE_ACTION_PYTHON(action)  \
	do \
	{ \
		LangAddress __retAddr__ = nullAddr; \
		if (gblOptions->pythonStackEnum == STACK_MODE_ENTER_EXIT_FUNC) { \
			__retAddr__ = env.getLocalProfiler().getBacktracePythonStack().getCurrentFrameAddr(); \
		} \
		if (gblOptions->pythonStackEnum == STACK_MODE_ENTER_EXIT_FUNC)\
		{\
			env.getLocalProfiler().onEnterFunc(nullAddr,__retAddr__,true); \
		}\
		do{action;}while(0); \
		if (gblOptions->pythonStackEnum == STACK_MODE_ENTER_EXIT_FUNC) \
			env.getLocalProfiler().onExitFunc(nullAddr,__retAddr__,true); \
	} while(0);

/**********************************************************/
namespace MALT
{

/**********************************************************/
void * malt_wrap_python_malloc(void * ctx, size_t size, PythonMallocFuncPtr real_malloc, void * retaddr)
{
	//get local TLS and check init
	LazyEnv env;
	ReentranceGuard guard(env);

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	
	//real call
	ticks t = Clock::getticks();
	void * res = real_malloc(ctx, size);
	t = Clock::getticks() - t;

	//profile
	if (guard.needInstrument()) {
		MALT_WRAPPER_LOCAL_STATE_ACTION_PYTHON(env.getLocalProfiler().onMalloc(res,size,t,MALLOC_KIND_MALLOC,LANG_PYTHON));
	}

	//return segment to user
	return res;
}

/**********************************************************/
void malt_wrap_python_free(void * ctx, void * ptr, PythonFreeFuncPtr real_free, void * retaddr)
{
	//get local TLS and check init
	LazyEnv env;
	ReentranceGuard guard(env);

	//profile
	if (guard.needInstrument()) {
		MALT_WRAPPER_LOCAL_STATE_ACTION_PYTHON(env.getLocalProfiler().onFree(ptr,0,LANG_PYTHON));
	}

	//run the default function
	assert(env.getGlobalState().status > ALLOC_WRAP_INIT_SYM);
	real_free(ctx, ptr);
}

/**********************************************************/
void * malt_wrap_python_calloc(void * ctx, size_t nmemb,size_t size, PythonCallocFuncPtr real_calloc, void * retaddr)
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
	void * res = real_calloc(ctx,nmemb,size);
	t = Clock::getticks() - t;

	//profile
	if (guard.needInstrument()) {
		MALT_WRAPPER_LOCAL_STATE_ACTION_PYTHON(env.getLocalProfiler().onCalloc(res,nmemb,size,t,LANG_PYTHON));
	}

	//return result to user
	return res;
}

/**********************************************************/
void * malt_wrap_python_realloc(void * ctx,void * ptr,size_t size, PythonReallocFuncPtr real_realloc, void * retaddr)
{
	//get local TLS and check init
	LazyEnv env;
	ReentranceGuard guard(env);

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	
	ticks t = Clock::getticks();
	void * res = real_realloc(ctx,ptr,size);
	t = Clock::getticks() - t;
	
	//profile
	if (guard.needInstrument()) {
		MALT_WRAPPER_LOCAL_STATE_ACTION_PYTHON(env.getLocalProfiler().onRealloc(ptr,res,size,t,LANG_PYTHON));
	}
	
	return res;
}

/**********************************************************/
int malt_wrap_python_on_enter_exit(PyObject *obj, PyFrameObject *frame, int what, PyObject *arg)
{
	//get local TLS and check init
	LazyEnv env;
	ReentranceGuard guard(env);

	//apply
	if (guard.needInstrument()) {
		switch (what)
		{
			case PyTrace_CALL: {
				MALT::PythonSymbolTracker & tracker = gblState.profiler->getPythonSymbolTracker();
				LangAddress parentAddr = tracker.parentFrameToLangAddress(frame);
				//get up
				PythonCallSite site = tracker.getCallSite(parentAddr);
				env.getLocalProfiler().onEnterFunc(nullAddr,parentAddr,true);
				printf("enter in %s:%s:%d\n", site.file, site.function, site.line);
				//////////////////////
				/*PyFrameObject * parentFrame = PyFrame_GetBack(frame);
				if (parentFrame != nullptr) {
					env.getLocalProfiler().onEnterFunc(nullAddr,LangAddress(DOMAIN_PYTHON_FRAME, parentFrame),true);
					//Py_DECREF(parentFrame);
				}*/

				break;
			}
			case PyTrace_RETURN: {
				MALT::PythonSymbolTracker & tracker = gblState.profiler->getPythonSymbolTracker();
				LangAddress parentAddr = tracker.parentFrameToLangAddress(frame);
				PythonCallSite site = tracker.getCallSite(parentAddr);
				env.getLocalProfiler().onExitFunc(nullAddr,parentAddr,true);
				printf("exit in %s:%s:%d\n", site.file, site.function, site.line);
				//////////////
				/*PyFrameObject * parentFrame = PyFrame_GetBack(frame);
				if (parentFrame != nullptr) {
					env.getLocalProfiler().onExitFunc(nullAddr,LangAddress(DOMAIN_PYTHON_FRAME, parentFrame),true);
					//Py_DECREF(parentFrame);
				}*/
				break;
			}
			/*case PyTrace_C_CALL:
				env.getLocalProfiler().onEnterFunc(nullAddr, LangAddress(DOMAIN_PYTHON, MALT_PYTHON_C_BRIDGE_FUNC_ID),true);
				break;
			case PyTrace_C_RETURN:
				env.getLocalProfiler().onExitFunc(nullAddr, LangAddress(DOMAIN_PYTHON, MALT_PYTHON_C_BRIDGE_FUNC_ID),true);
				break;*/
			default:{
				//ignored
			}
		}
	}

	//ok
	return 0;
}

}

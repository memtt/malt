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
#include "portability/Python.hpp"
#include "state/LazyEnv.hpp"
#include "state/ReentranceGuard.hpp"
#include "WrapperPython.hpp"
#include <stacks/BacktracePythonStack.hpp>
#include "core/InternalLeakTracker.hpp"

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

//static InternalLeakTracker gblLeakTracker(false);

/**********************************************************/
void * malt_wrap_python_malloc(void * ctx, size_t size, PythonMallocFuncPtr real_malloc, void * retaddr)
{
	//get local TLS and check init
	LazyEnv env;
	ReentranceGuard guard(env);

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);

	//profile
	void * res = nullptr;
	if (guard.needInstrument()) {
		//real call
		ticks t = Clock::getticks();
		res = real_malloc(ctx, size);
		t = Clock::getticks() - t;

		MALT_WRAPPER_LOCAL_STATE_ACTION_PYTHON(env.getLocalProfiler().onMalloc(res,size,t,MALLOC_KIND_MALLOC,LANG_PYTHON));
	} else {
		res = real_malloc(ctx, size);
		//gblLeakTracker.onMalloc(res, size);
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

		//run the default function
		assert(env.getGlobalState().status > ALLOC_WRAP_INIT_SYM);
		real_free(ctx, ptr);
	} else {
		//gblLeakTracker.onFree(ptr);
		real_free(ctx, ptr);
	}
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

	//profile
	void * res = nullptr;
	if (guard.needInstrument()) {
		ticks t = Clock::getticks();
		res = real_calloc(ctx,nmemb,size);
		t = Clock::getticks() - t;

		MALT_WRAPPER_LOCAL_STATE_ACTION_PYTHON(env.getLocalProfiler().onCalloc(res,nmemb,size,t,LANG_PYTHON));
	} else {
		res = real_calloc(ctx,nmemb,size);
		//gblLeakTracker.onMalloc(res, nmemb*size);
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
	
	//profile
	void * res = nullptr;
	if (guard.needInstrument()) {
		ticks t = Clock::getticks();
		res = real_realloc(ctx,ptr,size);
		t = Clock::getticks() - t;

		MALT_WRAPPER_LOCAL_STATE_ACTION_PYTHON(env.getLocalProfiler().onRealloc(ptr,res,size,t,LANG_PYTHON));
	} else {
		res = real_realloc(ctx,ptr,size);
		//gblLeakTracker.onRealloc(res, size);
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
		/*static Py_tss_t key = Py_tss_NEEDS_INIT;
		static bool isInit = false;
		if (!isInit)
			MALT::PyThread_tss_create(&key);
		void * pptr = MALT::PyThread_tss_get(&key);
		if (pptr == nullptr) {
			pptr = MALT_MALLOC(1);
			MALT::PyThread_tss_set(&key, pptr);
			fprintf(stderr, "MALT: Create thread : %p\n", pptr);
		} else {
			//fprintf(stderr, "MALT: Thread is : %p\n", pptr);
		}*/

		switch (what)
		{
			case PyTrace_CALL: {
				MALT::PythonSymbolTracker & tracker = gblState.profiler->getPythonSymbolTracker();
				LangAddress parentAddr = tracker.parentFrameToLangAddress(frame);
				//////////////////////
				//PythonNamedCallSite site = tracker.getNamedCallSite(parentAddr);
				//printf("enter in %s:%s:%d\n", site.file, site.function, site.line);
				//////////////////////
				//get up
				env.getLocalProfiler().onEnterFunc(nullAddr,parentAddr,true);
				/*PyFrameObject * parentFrame = PyFrame_GetBack(frame);
				if (parentFrame != nullptr) {
					env.getLocalProfiler().onEnterFunc(nullAddr,LangAddress(DOMAIN_PYTHON_FRAME, parentFrame),true);
					//Py_DecRef(parentFrame);
				}*/

				break;
			}
			case PyTrace_RETURN: {
				MALT::PythonSymbolTracker & tracker = gblState.profiler->getPythonSymbolTracker();
				LangAddress parentAddr = tracker.parentFrameToLangAddress(frame);
				//////////////
				//PythonNamedCallSite site = tracker.getNamedCallSite(parentAddr);
				//printf("exit in %s:%s:%d\n", site.file, site.function, site.line);
				//////////////
				env.getLocalProfiler().onExitFunc(nullAddr,parentAddr,true);
				/*PyFrameObject * parentFrame = PyFrame_GetBack(frame);
				if (parentFrame != nullptr) {
					env.getLocalProfiler().onExitFunc(nullAddr,LangAddress(DOMAIN_PYTHON_FRAME, parentFrame),true);
					//Py_DecRef(parentFrame);
				}*/
				break;
			}
			/*case PyTrace_EXCEPTION: {
				printf("EXCEPTION\n");
				break;
			};
			case PyTrace_C_CALL: {
				printf("C CALL\n");
				break;
			}
			case PyTrace_C_RETURN: {
				printf("C RETURN\n");
				break;
			}
			case PyTrace_C_EXCEPTION: {
				printf("C EXCEPTION\n");
				break;
			}*/
			/*case PyTrace_C_CALL:
				env.getLocalProfiler().onEnterFunc(nullAddr, LangAddress(DOMAIN_PYTHON, MALT_PYTHON_C_BRIDGE_FUNC_ID),true);
				break;
			case PyTrace_C_RETURN:
			case PyTrace_C_EXCEPTION:
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

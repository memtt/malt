/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 10/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/injectors/InjectPython.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include "portability/Python.hpp"
#include "state/GlobalState.hpp"
#include "wrappers/WrapperPython.hpp"
#include "InjectPython.hpp"
#include "state/LazyEnv.hpp"
#include "state/ReentranceGuard.hpp"
#include "portability/Visibility.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
PyMemAllocatorEx gblPythonRawAlloc;
PyMemAllocatorEx gblPythonMemAlloc;
PyMemAllocatorEx gblPythonObjAlloc;

/**********************************************************/
void* WrapperPythonRaw::malloc(void* ctx, size_t size)
{
	return malt_wrap_python_malloc(gblPythonRawAlloc.ctx, size, gblPythonRawAlloc.malloc, MALT_RETADDR, DOMAIN_PYTHON_RAW);
}

/**********************************************************/
void WrapperPythonRaw::free(void* ctx, void* ptr)
{
	if (ptr != nullptr)
		return malt_wrap_python_free(gblPythonRawAlloc.ctx, ptr, gblPythonRawAlloc.free, MALT_RETADDR, DOMAIN_PYTHON_RAW);
}

/**********************************************************/
void* WrapperPythonRaw::calloc(void* ctx, size_t nelem, size_t elsize)
{
	return malt_wrap_python_calloc(gblPythonRawAlloc.ctx, nelem, elsize, gblPythonRawAlloc.calloc, MALT_RETADDR, DOMAIN_PYTHON_RAW);
}

/**********************************************************/
void* WrapperPythonRaw::realloc(void* ctx, void *ptr, size_t new_size)
{
	return malt_wrap_python_realloc(gblPythonRawAlloc.ctx, ptr, new_size, gblPythonRawAlloc.realloc, MALT_RETADDR, DOMAIN_PYTHON_RAW);
}

/**********************************************************/
void* WrapperPythonMem::malloc(void* ctx, size_t size)
{
	return malt_wrap_python_malloc(gblPythonMemAlloc.ctx, size, gblPythonMemAlloc.malloc, MALT_RETADDR, DOMAIN_PYTHON_MEM);
}

/**********************************************************/
void WrapperPythonMem::free(void* ctx, void* ptr)
{
	return malt_wrap_python_free(gblPythonMemAlloc.ctx, ptr, gblPythonMemAlloc.free, MALT_RETADDR, DOMAIN_PYTHON_MEM);
}

/**********************************************************/
void* WrapperPythonMem::calloc(void* ctx, size_t nelem, size_t elsize)
{
	return malt_wrap_python_calloc(gblPythonMemAlloc.ctx, nelem, elsize, gblPythonMemAlloc.calloc, MALT_RETADDR, DOMAIN_PYTHON_MEM);
}

/**********************************************************/
void* WrapperPythonMem::realloc(void* ctx, void *ptr, size_t new_size)
{
	return malt_wrap_python_realloc(gblPythonMemAlloc.ctx, ptr, new_size, gblPythonMemAlloc.realloc, MALT_RETADDR, DOMAIN_PYTHON_MEM);
}

/**********************************************************/
void* WrapperPythonObj::malloc(void* ctx, size_t size)
{
	return malt_wrap_python_malloc(gblPythonObjAlloc.ctx, size, gblPythonObjAlloc.malloc, MALT_RETADDR, DOMAIN_PYTHON_OBJ);
}

/**********************************************************/
void WrapperPythonObj::free(void* ctx, void* ptr)
{
	return malt_wrap_python_free(gblPythonObjAlloc.ctx, ptr, gblPythonObjAlloc.free, MALT_RETADDR, DOMAIN_PYTHON_OBJ);
}

/**********************************************************/
void* WrapperPythonObj::calloc(void* ctx, size_t nelem, size_t elsize)
{
	return malt_wrap_python_calloc(gblPythonObjAlloc.ctx, nelem, elsize, gblPythonObjAlloc.calloc, MALT_RETADDR, DOMAIN_PYTHON_OBJ);
}

/**********************************************************/
void* WrapperPythonObj::realloc(void* ctx, void *ptr, size_t new_size)
{
	return malt_wrap_python_realloc(gblPythonObjAlloc.ctx, ptr, new_size, gblPythonObjAlloc.realloc, MALT_RETADDR, DOMAIN_PYTHON_OBJ);
}

/**********************************************************/
void initPythonAllocInstrumentation()
{
	if (gblOptions->outputVerbosity >= MALT_VERBOSITY_DEFAULT)
		printf("MALT: Instument Python allocator...\n");

	//get all
	MALT::PyMem_GetAllocator(PYMEM_DOMAIN_RAW, &gblPythonRawAlloc);
	MALT::PyMem_GetAllocator(PYMEM_DOMAIN_MEM, &gblPythonMemAlloc);
	MALT::PyMem_GetAllocator(PYMEM_DOMAIN_OBJ, &gblPythonObjAlloc);

	if (gblOptions->pythonRaw) {
		PyMemAllocatorEx pythonRawAllocMalt;
		pythonRawAllocMalt.ctx = NULL;
		pythonRawAllocMalt.malloc = WrapperPythonRaw::malloc;
		pythonRawAllocMalt.free = WrapperPythonRaw::free;
		pythonRawAllocMalt.calloc = WrapperPythonRaw::calloc;
		pythonRawAllocMalt.realloc = WrapperPythonRaw::realloc;
		MALT::PyMem_SetAllocator(PYMEM_DOMAIN_RAW, &pythonRawAllocMalt);
	}

	if (gblOptions->pythonMem) {
		PyMemAllocatorEx pythonMemAllocMalt;
		pythonMemAllocMalt.ctx = NULL;
		pythonMemAllocMalt.malloc = WrapperPythonMem::malloc;
		pythonMemAllocMalt.free = WrapperPythonMem::free;
		pythonMemAllocMalt.calloc = WrapperPythonMem::calloc;
		pythonMemAllocMalt.realloc = WrapperPythonMem::realloc;
		MALT::PyMem_SetAllocator(PYMEM_DOMAIN_MEM, &pythonMemAllocMalt);
	}

	if (gblOptions->pythonObj) {
		PyMemAllocatorEx pythonObjAllocMalt;
		pythonObjAllocMalt.ctx = NULL;
		pythonObjAllocMalt.malloc = WrapperPythonObj::malloc;
		pythonObjAllocMalt.free = WrapperPythonObj::free;
		pythonObjAllocMalt.calloc = WrapperPythonObj::calloc;
		pythonObjAllocMalt.realloc = WrapperPythonObj::realloc;
		MALT::PyMem_SetAllocator(PYMEM_DOMAIN_OBJ, &pythonObjAllocMalt);
	}

	//PyGC_Disable();

	//https://docs.python.org/3/c-api/memory.html
	/*
	void PyMem_GetAllocator(PyMemAllocatorDomain domain, PyMemAllocatorEx *allocator)
	void PyMem_SetAllocator(PyMemAllocatorDomain domain, PyMemAllocatorEx *allocator)
	PYMEM_DOMAIN_RAW¶
	PYMEM_DOMAIN_MEM
	PYMEM_DOMAIN_OBJ
	
	Py_Initialize()
	Py_IsInitialized()
	Py_IsFinalizing()
	Py_Finalize()
	Py_Main()
	Py_BytesMain()
	*/

	//idealement juste apres
	//pymain -> pymain_init -> Py_InitializeFromConfig -> _PyRuntime_Initialize
	//       -> Py_RunMain()
	
	//PYTHONMALLOC=debug ---- PyMem_SetupDebugHooks()

	//Create an exe => call 
	/*
	int main(int argc, char** argv)
	{
		return Py_BytesMain(argc, argv);
	}
	*/

	//Be init before (by wrapping) : int Py_RunMain()
}

/**********************************************************/
void finiPythonAllocInstrumentation(void)
{
	if (gblOptions->outputVerbosity >= MALT_VERBOSITY_DEFAULT)
		printf("MALT: Un-instument Python allocator...\n");
	MALT::PyMem_SetAllocator(PYMEM_DOMAIN_RAW, &gblPythonRawAlloc);
	MALT::PyMem_SetAllocator(PYMEM_DOMAIN_MEM, &gblPythonRawAlloc);
	MALT::PyMem_SetAllocator(PYMEM_DOMAIN_OBJ, &gblPythonMemAlloc);
}

/**********************************************************/
void initPythonEnterExitInstrumentation(void)
{
	if (gblOptions->outputVerbosity >= MALT_VERBOSITY_DEFAULT)
		printf("MALT: Instument Python profiling...\n");
	PyGILState_STATE gstate;
	gstate = MALT::PyGILState_Ensure();
	if (gblOptions->pythonMode == "profile") {
		MALT::PyEval_SetProfileAllThreads(malt_wrap_python_on_enter_exit, NULL);
	} else if (gblOptions->pythonMode == "trace") {
		MALT::PyEval_SetProfileAllThreads(malt_wrap_python_on_enter_exit, NULL);
		MALT::PyEval_SetTraceAllThreads(malt_wrap_python_on_enter_exit, NULL);
	} else {
		MALT_FATAL_ARG("Invalid value for option python:mode=%1").arg(gblOptions->pythonMode).end();
	}
	MALT::PyGILState_Release(gstate);
}

/**********************************************************/
void pythonOnExit(void)
{
	gblState.profiler->getPythonSymbolTracker().setPythonActivity(false);
	finiPythonAllocInstrumentation();
	if (gblOptions->outputVerbosity >= MALT_VERBOSITY_VERBOSE)
		printf("MALT: Python exit...\n");
}

/**********************************************************/
void initPythonInstrumentation(const char * script)
{
	//setup env
	LazyEnv env;
	ReentranceGuard guard(env);

	//init python
	initPythonAllocInstrumentation();
	if (gblOptions->pythonStackEnum == STACK_MODE_ENTER_EXIT_FUNC)
		initPythonEnterExitInstrumentation();

	assert(gblState.profiler != nullptr);
	if (script == nullptr)
		gblState.profiler->getPythonSymbolTracker().solveExeName();
	else
		gblState.profiler->getPythonSymbolTracker().setScript(script);

	//capture exit to stop some analyses
	gblState.profiler->getPythonSymbolTracker().setPythonActivity(true);
	MALT::Py_AtExit(pythonOnExit);
}

}

/**********************************************************/
/**
 * We need to override the GIL checking to get notified of new threads created
 * so register the stack tracking hooks.
 */
/*
DLL_PUBLIC PyGILState_STATE PyGILState_Ensure(void)
{
	//get previous state
	PyThreadState * state = MALT::PyGILState_GetThisThreadState();

	//get native API
	typedef PyGILState_STATE (*nativ_PyGILState_Ensure)(void);
	static nativ_PyGILState_Ensure native = NULL;
	if (native == NULL) {
		fprintf(stderr, "MALT: Register in thread !\n");
		native = (nativ_PyGILState_Ensure)dlsym(RTLD_NEXT, "PyGILState_Ensure");
	}

	//call
	PyGILState_STATE result = native();

	//register hooks
	if (state == NULL)
		MALT::PyEval_SetProfile(MALT::malt_wrap_python_on_enter_exit, NULL);
	return result;
}
*/

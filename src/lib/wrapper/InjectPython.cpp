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
#include "InjectPython.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
::PyMemAllocatorEx gblPythonRawAlloc;
::PyMemAllocatorEx gblPythonMemAlloc;
::PyMemAllocatorEx gblPythonObjAlloc;

/**********************************************************/
void* WrapperPythonRaw::malloc(void* ctx, size_t size)
{
	return malt_wrap_python_malloc(gblPythonRawAlloc.ctx, size, gblPythonRawAlloc.malloc, MALT_RETADDR);
}

/**********************************************************/
void WrapperPythonRaw::free(void* ctx, void* ptr)
{
	return malt_wrap_python_free(gblPythonRawAlloc.ctx, ptr, gblPythonRawAlloc.free, MALT_RETADDR);
}

/**********************************************************/
void* WrapperPythonRaw::calloc(void* ctx, size_t nelem, size_t elsize)
{
	return malt_wrap_python_calloc(gblPythonRawAlloc.ctx, nelem, elsize, gblPythonRawAlloc.calloc, MALT_RETADDR);
}

/**********************************************************/
void* WrapperPythonRaw::realloc(void* ctx, void *ptr, size_t new_size)
{
	return malt_wrap_python_realloc(gblPythonRawAlloc.ctx, ptr, new_size, gblPythonRawAlloc.realloc, MALT_RETADDR);
}

/**********************************************************/
void* WrapperPythonMem::malloc(void* ctx, size_t size)
{
	return malt_wrap_python_malloc(gblPythonMemAlloc.ctx, size, gblPythonMemAlloc.malloc, MALT_RETADDR);
}

/**********************************************************/
void WrapperPythonMem::free(void* ctx, void* ptr)
{
	return malt_wrap_python_free(gblPythonMemAlloc.ctx, ptr, gblPythonMemAlloc.free, MALT_RETADDR);
}

/**********************************************************/
void* WrapperPythonMem::calloc(void* ctx, size_t nelem, size_t elsize)
{
	return malt_wrap_python_calloc(gblPythonMemAlloc.ctx, nelem, elsize, gblPythonMemAlloc.calloc, MALT_RETADDR);
}

/**********************************************************/
void* WrapperPythonMem::realloc(void* ctx, void *ptr, size_t new_size)
{
	return malt_wrap_python_realloc(gblPythonMemAlloc.ctx, ptr, new_size, gblPythonMemAlloc.realloc, MALT_RETADDR);
}

/**********************************************************/
void* WrapperPythonObj::malloc(void* ctx, size_t size)
{
	return malt_wrap_python_malloc(gblPythonObjAlloc.ctx, size, gblPythonObjAlloc.malloc, MALT_RETADDR);
}

/**********************************************************/
void WrapperPythonObj::free(void* ctx, void* ptr)
{
	return malt_wrap_python_free(gblPythonObjAlloc.ctx, ptr, gblPythonObjAlloc.free, MALT_RETADDR);
}

/**********************************************************/
void* WrapperPythonObj::calloc(void* ctx, size_t nelem, size_t elsize)
{
	return malt_wrap_python_calloc(gblPythonObjAlloc.ctx, nelem, elsize, gblPythonObjAlloc.calloc, MALT_RETADDR);
}

/**********************************************************/
void* WrapperPythonObj::realloc(void* ctx, void *ptr, size_t new_size)
{
	return malt_wrap_python_realloc(gblPythonObjAlloc.ctx, ptr, new_size, gblPythonObjAlloc.realloc, MALT_RETADDR);
}

/**********************************************************/
void initPythonAllocInstrumentation()
{
	printf("MALT: Instument Python allocator...\n");

	//get all
	PyMem_GetAllocator(::PYMEM_DOMAIN_RAW, &gblPythonRawAlloc);
	PyMem_GetAllocator(::PYMEM_DOMAIN_MEM, &gblPythonMemAlloc);
	PyMem_GetAllocator(::PYMEM_DOMAIN_OBJ, &gblPythonObjAlloc);

	::PyMemAllocatorEx pythonRawAllocMalt;
	pythonRawAllocMalt.ctx = NULL;
	pythonRawAllocMalt.malloc = WrapperPythonRaw::malloc;
	pythonRawAllocMalt.free = WrapperPythonRaw::free;
	pythonRawAllocMalt.calloc = WrapperPythonRaw::calloc;
	pythonRawAllocMalt.realloc = WrapperPythonRaw::realloc;
	PyMem_SetAllocator(::PYMEM_DOMAIN_RAW, &pythonRawAllocMalt);

	::PyMemAllocatorEx pythonMemAllocMalt;
	pythonMemAllocMalt.ctx = NULL;
	pythonMemAllocMalt.malloc = WrapperPythonMem::malloc;
	pythonMemAllocMalt.free = WrapperPythonMem::free;
	pythonMemAllocMalt.calloc = WrapperPythonMem::calloc;
	pythonMemAllocMalt.realloc = WrapperPythonMem::realloc;
	PyMem_SetAllocator(::PYMEM_DOMAIN_MEM, &pythonMemAllocMalt);

	::PyMemAllocatorEx pythonObjAllocMalt;
	pythonObjAllocMalt.ctx = NULL;
	pythonObjAllocMalt.malloc = WrapperPythonObj::malloc;
	pythonObjAllocMalt.free = WrapperPythonObj::free;
	pythonObjAllocMalt.calloc = WrapperPythonObj::calloc;
	pythonObjAllocMalt.realloc = WrapperPythonObj::realloc;
	PyMem_SetAllocator(::PYMEM_DOMAIN_OBJ, &pythonObjAllocMalt);

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
void initPythonEnterExitInstrumentation(void)
{
	printf("MALT: Instument Python profiling...\n");
	PyEval_SetProfileAllThreads(malt_wrap_python_on_enter_exit, NULL);
}

/**********************************************************/
void initPythonInstrumentation(void)
{
	initPythonAllocInstrumentation();
	initPythonEnterExitInstrumentation();
}

}

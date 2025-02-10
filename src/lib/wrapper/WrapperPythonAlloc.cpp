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
#include "GlobalState.hpp"
#include "WrapperPythonAlloc.hpp"
#include <Python.h>

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
	printf("Python malloc RAW\n");
	return gblPythonRawAlloc.malloc(gblPythonRawAlloc.ctx, size);
}

/**********************************************************/
void WrapperPythonRaw::free(void* ctx, void* ptr)
{
	printf("Python free RAW\n");
	return gblPythonRawAlloc.free(gblPythonRawAlloc.ctx, ptr);

}

/**********************************************************/
void* WrapperPythonRaw::calloc(void* ctx, size_t nelem, size_t elsize)
{
	printf("Python calloc RAW\n");
	return gblPythonRawAlloc.calloc(gblPythonRawAlloc.ctx, nelem, elsize);
}

/**********************************************************/
void* WrapperPythonRaw::realloc(void* ctx, void *ptr, size_t new_size)
{
	printf("Python realloc RAW\n");
	return gblPythonRawAlloc.realloc(gblPythonRawAlloc.ctx, ptr, new_size);
}

/**********************************************************/
void* WrapperPythonMem::malloc(void* ctx, size_t size)
{
	printf("Python malloc Mem\n");
	return gblPythonMemAlloc.malloc(gblPythonMemAlloc.ctx, size);
}

/**********************************************************/
void WrapperPythonMem::free(void* ctx, void* ptr)
{
	printf("Python free Mem\n");
	return gblPythonMemAlloc.free(gblPythonMemAlloc.ctx, ptr);

}

/**********************************************************/
void* WrapperPythonMem::calloc(void* ctx, size_t nelem, size_t elsize)
{
	printf("Python calloc Mem\n");
	return gblPythonMemAlloc.calloc(gblPythonMemAlloc.ctx, nelem, elsize);
}

/**********************************************************/
void* WrapperPythonMem::realloc(void* ctx, void *ptr, size_t new_size)
{
	printf("Python realloc Mem\n");
	return gblPythonMemAlloc.realloc(gblPythonMemAlloc.ctx, ptr, new_size);
}

/**********************************************************/
void* WrapperPythonObj::malloc(void* ctx, size_t size)
{
	printf("Python malloc Obj\n");
	return gblPythonObjAlloc.malloc(gblPythonObjAlloc.ctx, size);
}

/**********************************************************/
void WrapperPythonObj::free(void* ctx, void* ptr)
{
	printf("Python free Obj\n");
	return gblPythonObjAlloc.free(gblPythonObjAlloc.ctx, ptr);

}

/**********************************************************/
void* WrapperPythonObj::calloc(void* ctx, size_t nelem, size_t elsize)
{
	printf("Python calloc Obj\n");
	return gblPythonObjAlloc.calloc(gblPythonObjAlloc.ctx, nelem, elsize);
}

/**********************************************************/
void* WrapperPythonObj::realloc(void* ctx, void *ptr, size_t new_size)
{
	printf("Python realloc Obj\n");
	return gblPythonObjAlloc.realloc(gblPythonObjAlloc.ctx, ptr, new_size);
}

/**********************************************************/
void initPythonAllocInstrumentation()
{
	printf("MALT PYTHON INIT <================\n");

	//get all
	PyMem_GetAllocator(::PYMEM_DOMAIN_RAW, &gblPythonRawAlloc);
	PyMem_GetAllocator(::PYMEM_DOMAIN_MEM, &gblPythonMemAlloc);
	PyMem_GetAllocator(::PYMEM_DOMAIN_OBJ, &gblPythonObjAlloc);

	::PyMemAllocatorEx pythonRawAllocMalt;
	pythonRawAllocMalt.malloc = WrapperPythonRaw::malloc;
	pythonRawAllocMalt.free = WrapperPythonRaw::free;
	pythonRawAllocMalt.calloc = WrapperPythonRaw::calloc;
	pythonRawAllocMalt.realloc = WrapperPythonRaw::realloc;
	PyMem_SetAllocator(::PYMEM_DOMAIN_RAW, &pythonRawAllocMalt);

	::PyMemAllocatorEx pythonMemAllocMalt;
	pythonMemAllocMalt.malloc = WrapperPythonMem::malloc;
	pythonMemAllocMalt.free = WrapperPythonMem::free;
	pythonMemAllocMalt.calloc = WrapperPythonMem::calloc;
	pythonMemAllocMalt.realloc = WrapperPythonMem::realloc;
	PyMem_SetAllocator(::PYMEM_DOMAIN_MEM, &pythonMemAllocMalt);

	::PyMemAllocatorEx pythonObjAllocMalt;
	pythonObjAllocMalt.malloc = WrapperPythonObj::malloc;
	pythonObjAllocMalt.free = WrapperPythonObj::free;
	pythonObjAllocMalt.calloc = WrapperPythonObj::calloc;
	pythonObjAllocMalt.realloc = WrapperPythonObj::realloc;
	PyMem_SetAllocator(::PYMEM_DOMAIN_OBJ, &pythonObjAllocMalt);

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

}
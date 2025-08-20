/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 06/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/portability/PythonLazy.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <dlfcn.h>
#include "PythonLazy.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
static PythonAPIFuncPtrs gblPythonApi;

/**********************************************************/
#define MALT_PYTHON_DLSYM(symbolName, supported) \
	do { \
		gblPythonApi.symbolName = (symbolName##FuncPtr)dlsym(RTLD_DEFAULT,#symbolName); \
		if (gblPythonApi.symbolName == nullptr) { \
			/*fprintf(stderr, "MALT: Fail loading Python symbol " #symbolName "\n");*/ \
			dlerror(); \
			supported = false; \
		} \
	} while(0)

/**********************************************************/
bool PyLazyInterfaceInit(void)
{
	bool supported = true;
	//fprintf(stderr, "MALT: Loading python symbols...\n");
	//MALT_PYTHON_DLSYM(_PyThreadState_UncheckedGet, supported);
	MALT_PYTHON_DLSYM(PyThreadState_GetFrame, supported);
	MALT_PYTHON_DLSYM(PyGILState_GetThisThreadState, supported);
	MALT_PYTHON_DLSYM(PyFrame_GetBack, supported);
	MALT_PYTHON_DLSYM(PyMem_GetAllocator, supported);
	MALT_PYTHON_DLSYM(PyMem_SetAllocator, supported);
	MALT_PYTHON_DLSYM(PyGILState_Ensure, supported);
	MALT_PYTHON_DLSYM(PyGILState_Release, supported);
	MALT_PYTHON_DLSYM(PyEval_SetProfile, supported);
	MALT_PYTHON_DLSYM(PyEval_SetProfileAllThreads, supported);
	MALT_PYTHON_DLSYM(PyEval_SetTraceAllThreads, supported);
	MALT_PYTHON_DLSYM(PyFrame_GetCode, supported);
	MALT_PYTHON_DLSYM(PyUnicode_AsASCIIString, supported);
	MALT_PYTHON_DLSYM(PyBytes_AsString, supported);
	MALT_PYTHON_DLSYM(PyFrame_GetLineNumber, supported);
	MALT_PYTHON_DLSYM(PyObject_Free, supported);
	MALT_PYTHON_DLSYM(Py_BytesMain, supported);
	MALT_PYTHON_DLSYM(PyUnicode_AsEncodedString, supported);
	MALT_PYTHON_DLSYM(Py_IsInitialized, supported);
	MALT_PYTHON_DLSYM(Py_DecRef, supported);

	MALT_PYTHON_DLSYM(PySys_GetObject, supported);
	MALT_PYTHON_DLSYM(PyList_GetItem, supported);
	MALT_PYTHON_DLSYM(PyImport_ImportModule, supported);
	MALT_PYTHON_DLSYM(Py_AtExit, supported);
	
	//MALT_PYTHON_DLSYM(Py_RefCnt, supported);
	/*MALT_PYTHON_DLSYM(PyThread_tss_create, supported);
	MALT_PYTHON_DLSYM(PyThread_tss_set, supported);
	MALT_PYTHON_DLSYM(PyThread_tss_get, supported);*/
	//fprintf(stderr, "MALT: Python support = %d\n", (int)supported);
	return supported;
}

/**********************************************************/
/*void * _PyThreadState_UncheckedGet()
{
	if (gblPythonApi._PyThreadState_UncheckedGet != nullptr)
		return gblPythonApi._PyThreadState_UncheckedGet();
	else
		return 0;
}*/

/**********************************************************/
PyFrameObject * PyThreadState_GetFrame(PyThreadState * gilState)
{
	if (gblPythonApi.PyThreadState_GetFrame != nullptr)
		return gblPythonApi.PyThreadState_GetFrame(gilState);
	else
		return nullptr;
}

/**********************************************************/
PyThreadState * PyGILState_GetThisThreadState()
{
	if (gblPythonApi.PyGILState_GetThisThreadState != nullptr)
		return gblPythonApi.PyGILState_GetThisThreadState();
	else
		return nullptr;
}

/**********************************************************/
PyFrameObject * PyFrame_GetBack(PyFrameObject * frame)
{
	if (gblPythonApi.PyFrame_GetBack != nullptr)
		return gblPythonApi.PyFrame_GetBack(frame);
	else
		return nullptr;
}

/**********************************************************/
void PyMem_GetAllocator(PyMemAllocatorDomain domain, PyMemAllocatorEx *allocator)
{
	if (gblPythonApi.PyMem_GetAllocator != nullptr)
		gblPythonApi.PyMem_GetAllocator(domain, allocator);
}

/**********************************************************/
void PyMem_SetAllocator(PyMemAllocatorDomain domain, PyMemAllocatorEx *allocator)
{
	if (gblPythonApi.PyMem_SetAllocator != nullptr)
		gblPythonApi.PyMem_SetAllocator(domain, allocator);
}

/**********************************************************/
PyGILState_STATE PyGILState_Ensure(void)
{
	if (gblPythonApi.PyGILState_Ensure != nullptr)
		return gblPythonApi.PyGILState_Ensure();
	else
		return PyGILState_UNLOCKED;
}

/**********************************************************/
void PyGILState_Release(PyGILState_STATE state)
{
	if (gblPythonApi.PyGILState_Release != nullptr)
		gblPythonApi.PyGILState_Release(state);
}

/**********************************************************/
void PyEval_SetProfile(Py_tracefunc func, PyObject *obj)
{
	if (gblPythonApi.PyEval_SetProfile != nullptr)
		gblPythonApi.PyEval_SetProfile(func, obj);
}

/**********************************************************/
void PyEval_SetProfileAllThreads(Py_tracefunc func, PyObject *obj)
{
	if (gblPythonApi.PyEval_SetProfileAllThreads != nullptr)
		gblPythonApi.PyEval_SetProfileAllThreads(func, obj);
}

/**********************************************************/
void PyEval_SetTraceAllThreads(Py_tracefunc func, PyObject *obj)
{
	if (gblPythonApi.PyEval_SetTraceAllThreads != nullptr)
		gblPythonApi.PyEval_SetTraceAllThreads(func, obj);
}

/**********************************************************/
PyCodeObject * PyFrame_GetCode(PyFrameObject * frame)
{
	if (gblPythonApi.PyFrame_GetCode != nullptr)
		return gblPythonApi.PyFrame_GetCode(frame);
	else
		return nullptr;
}

/**********************************************************/
PyObject * PyUnicode_AsASCIIString(const char * value)
{
	if (gblPythonApi.PyUnicode_AsASCIIString != nullptr)
		return gblPythonApi.PyUnicode_AsASCIIString(value);
	else
		return nullptr;
}

/**********************************************************/
char * PyBytes_AsString(PyObject * obj)
{
	if (gblPythonApi.PyBytes_AsString != nullptr)
		return gblPythonApi.PyBytes_AsString(obj);
	else
		return nullptr;
}

/**********************************************************/
int PyFrame_GetLineNumber(PyFrameObject * frame)
{
	if (gblPythonApi.PyFrame_GetLineNumber != nullptr)
		return gblPythonApi.PyFrame_GetLineNumber(frame);
	else
		return -1;
}

/**********************************************************/
void PyObject_Free(void * object)
{
	if (gblPythonApi.PyObject_Free != nullptr)
		gblPythonApi.PyObject_Free(object);
}

/**********************************************************/
int Py_BytesMain(int argc, char ** argv)
{
	if (gblPythonApi.Py_BytesMain != nullptr)
		return gblPythonApi.Py_BytesMain(argc, argv);
	else
		return EXIT_SUCCESS;
}

/**********************************************************/
PyObject * PyUnicode_AsEncodedString(PyObject * value, const char * encoding, const char * mode)
{
	if (gblPythonApi.PyUnicode_AsEncodedString != nullptr)
		return gblPythonApi.PyUnicode_AsEncodedString(value, encoding, mode);
	else
		return nullptr;
}

/**********************************************************/
int Py_IsInitialized(void)
{
	if (gblPythonApi.Py_IsInitialized != nullptr) {
		return gblPythonApi.Py_IsInitialized();
	} else {
		return 0;
	}
}

/**********************************************************/
void Py_Initialize(void)
{
	if (gblPythonApi.Py_Initialize != nullptr) {
		fprintf(stderr, "Init python;...\n");
		return gblPythonApi.Py_Initialize();
	}
}

/**********************************************************/
void Py_DecRef(PyObject* ptr)
{
	if (gblPythonApi.Py_DecRef != nullptr)
		return gblPythonApi.Py_DecRef(ptr);
}

/**********************************************************/
Py_ssize_t Py_RefCnt(PyObject* ptr)
{
	if (gblPythonApi.Py_RefCnt != nullptr)
		return gblPythonApi.Py_RefCnt(ptr);
	else
		return 1;
}

/**********************************************************/
PyObject *PySys_GetObject(const char *name)
{
	if (gblPythonApi.PySys_GetObject != nullptr)
		return gblPythonApi.PySys_GetObject(name);
	else
		return nullptr;
}

/**********************************************************/
PyObject *PyList_GetItem(PyObject *list, Py_ssize_t index)
{
	if (gblPythonApi.PyList_GetItem != nullptr)
		return gblPythonApi.PyList_GetItem(list, index);
	else
		return nullptr;
}

/**********************************************************/
PyObject *PyImport_ImportModule(const char *name)
{
	if (gblPythonApi.PyImport_ImportModule != nullptr)
		return gblPythonApi.PyImport_ImportModule(name);
	else
		return nullptr;
}

/**********************************************************/
int Py_AtExit(void (*func)())
{
	if (gblPythonApi.Py_AtExit != nullptr)
		return gblPythonApi.Py_AtExit(func);
	else
		return 0;
}

/**********************************************************/
/*int PyThread_tss_create(Py_tss_t *key)
{
	if (gblPythonApi.PyThread_tss_create != nullptr)
		return gblPythonApi.PyThread_tss_create(key);
	else
		return 1;
}*/

/**********************************************************/
/*int PyThread_tss_set(Py_tss_t *key, void *value)
{
	if (gblPythonApi.PyThread_tss_set != nullptr)
		return gblPythonApi.PyThread_tss_set(key, value);
	else
		return 1;
}*/

/**********************************************************/
/*void *PyThread_tss_get(Py_tss_t *key)
{
	if (gblPythonApi.PyThread_tss_get != nullptr)
		return gblPythonApi.PyThread_tss_get(key);
	else
		return nullptr;
}*/

}

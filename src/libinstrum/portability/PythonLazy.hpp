/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/common/Debug.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2015 - 2024
***********************************************************/

#ifndef MALT_PYTHON_LAZY_HPP
#define MALT_PYTHON_LAZY_HPP

/**********************************************************/
#include <cstdlib>
#include <cstdio>
#include <Python.h>
//for python 3.6 under Rocky 8.9
#ifndef PyFrameObject
	#include <frameobject.h>
#endif

/**********************************************************/
namespace MALT
{

/**********************************************************/
//typedef void * (*_PyThreadState_UncheckedGetFuncPtr)();
typedef PyFrameObject * (*PyThreadState_GetFrameFuncPtr)(PyThreadState * /*gilState*/);
typedef PyThreadState * (*PyGILState_GetThisThreadStateFuncPtr)();
typedef PyFrameObject * (*PyFrame_GetBackFuncPtr)(PyFrameObject * /*frame*/);
typedef void (*PyMem_GetAllocatorFuncPtr)(PyMemAllocatorDomain /*domain*/, PyMemAllocatorEx * /*allocator*/);
typedef void (*PyMem_SetAllocatorFuncPtr)(PyMemAllocatorDomain /*domain*/, PyMemAllocatorEx * /*allocator*/);
typedef PyGILState_STATE (*PyGILState_EnsureFuncPtr)(void);
typedef void (*PyGILState_ReleaseFuncPtr)(PyGILState_STATE /*state*/);
typedef void (*PyEval_SetProfileAllThreadsFuncPtr)(Py_tracefunc /*func*/, PyObject * /*obj*/);
typedef void (*PyEval_SetTraceAllThreadsFuncPtr)(Py_tracefunc /*func*/, PyObject * /*obj*/);
typedef PyCodeObject * (*PyFrame_GetCodeFuncPtr)(PyFrameObject * /*frame*/);
typedef PyObject * (*PyUnicode_AsASCIIStringFuncPtr)(const char * /*value*/);
typedef char * (*PyBytes_AsStringFuncPtr)(PyObject * /*obj*/);
typedef int (*PyFrame_GetLineNumberFuncPtr)(PyFrameObject * /*frame*/);
typedef void (*PyObject_FreeFuncPtr)(void * /*object*/);
typedef int (*Py_BytesMainFuncPtr)(int /*argc*/, char ** /*argv*/);
typedef PyObject * (*PyUnicode_AsEncodedStringFuncPtr)(PyObject * /*value*/, const char * /*encoding*/, const char * /*mode*/);
typedef int (*Py_IsInitializedFuncPtr)(void);
typedef void (*Py_DecRefFuncPtr)(PyObject* /*ptr*/);
typedef Py_ssize_t (*Py_RefCntFuncPtr)(PyObject* /*ptr*/);
typedef void (*Py_InitializeFuncPtr)(void);
//
//typedef int (*PyThread_tss_createFuncPtr)(Py_tss_t */*key*/);
//typedef int (*PyThread_tss_setFuncPtr)(Py_tss_t */*key*/, void */*value*/);
//typedef void *(*PyThread_tss_getFuncPtr)(Py_tss_t */*key*/);


/**********************************************************/
struct PythonAPIFuncPtrs
{
	//_PyThreadState_UncheckedGetFuncPtr _PyThreadState_UncheckedGet{nullptr};
	PyThreadState_GetFrameFuncPtr PyThreadState_GetFrame{nullptr};
	PyGILState_GetThisThreadStateFuncPtr PyGILState_GetThisThreadState{nullptr};
	PyFrame_GetBackFuncPtr PyFrame_GetBack{nullptr};
	PyMem_GetAllocatorFuncPtr PyMem_GetAllocator{nullptr};
	PyMem_SetAllocatorFuncPtr PyMem_SetAllocator{nullptr};
	PyGILState_EnsureFuncPtr PyGILState_Ensure{nullptr};
	PyGILState_ReleaseFuncPtr PyGILState_Release{nullptr};
	PyEval_SetProfileAllThreadsFuncPtr PyEval_SetProfileAllThreads{nullptr};
	PyEval_SetTraceAllThreadsFuncPtr PyEval_SetTraceAllThreads{nullptr};
	PyFrame_GetCodeFuncPtr PyFrame_GetCode{nullptr};
	PyUnicode_AsASCIIStringFuncPtr PyUnicode_AsASCIIString{nullptr};
	PyBytes_AsStringFuncPtr PyBytes_AsString{nullptr};
	PyFrame_GetLineNumberFuncPtr PyFrame_GetLineNumber{nullptr};
	PyObject_FreeFuncPtr PyObject_Free{nullptr};
	Py_BytesMainFuncPtr Py_BytesMain{nullptr};
	PyUnicode_AsEncodedStringFuncPtr PyUnicode_AsEncodedString{nullptr};
	Py_IsInitializedFuncPtr Py_IsInitialized{nullptr};
	Py_DecRefFuncPtr Py_DecRef{nullptr};
	Py_RefCntFuncPtr Py_RefCnt{nullptr};
	Py_InitializeFuncPtr Py_Initialize{nullptr};
	//
	PyThread_tss_createFuncPtr PyThread_tss_create{nullptr};
	PyThread_tss_setFuncPtr PyThread_tss_set{nullptr};
	PyThread_tss_getFuncPtr PyThread_tss_get{nullptr};
};

/**********************************************************/
bool PyLazyInterfaceInit(void);

/**********************************************************/
//void * _PyThreadState_UncheckedGet();
PyFrameObject * PyThreadState_GetFrame(PyThreadState * gilState);
PyThreadState * PyGILState_GetThisThreadState();
PyFrameObject * PyFrame_GetBack(PyFrameObject * frame);
void PyMem_GetAllocator(PyMemAllocatorDomain domain, PyMemAllocatorEx *allocator);
void PyMem_SetAllocator(PyMemAllocatorDomain domain, PyMemAllocatorEx *allocator);
PyGILState_STATE PyGILState_Ensure(void);
void PyGILState_Release(PyGILState_STATE state);
void PyEval_SetProfileAllThreads(Py_tracefunc func, PyObject *obj);
void PyEval_SetTraceAllThreads(Py_tracefunc func, PyObject *obj);
PyCodeObject * PyFrame_GetCode(PyFrameObject * frame);
PyObject * PyUnicode_AsASCIIString(const char * value);
char * PyBytes_AsString(PyObject * obj);
int PyFrame_GetLineNumber(PyFrameObject * frame);
void PyObject_Free(void * object);
int Py_BytesMain(int argc, char ** argv);
PyObject * PyUnicode_AsEncodedString(PyObject *unicode, const char *encoding, const char *errors);
int Py_IsInitialized(void);
void Py_DecRef(PyObject* ptr);
Py_ssize_t Py_RefCnt(PyObject* ptr);
void Py_Initialize(void);
//
int PyThread_tss_create(Py_tss_t *key);
int PyThread_tss_set(Py_tss_t *key, void *value);
void *PyThread_tss_get(Py_tss_t *key);

}

#endif //MALT_PYTHON_LAZY_HPP

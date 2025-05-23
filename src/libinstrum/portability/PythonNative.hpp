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

#ifndef MALT_PYTHON_NATIVE_HPP
#define MALT_PYTHON_NATIVE_HPP

/**********************************************************/
#include <Python.h>

/**********************************************************/
namespace MALT
{

/**********************************************************/
inline bool PyLazyInterfaceInit(void){return false;};

/**********************************************************/
//inline void * _PyThreadState_UncheckedGet(){return ::_PyThreadState_UncheckedGet();};
inline PyFrameObject * PyThreadState_GetFrame(PyThreadState* gilState){return ::PyThreadState_GetFrame(gilState);};
inline PyThreadState* PyGILState_GetThisThreadState() {return ::PyGILState_GetThisThreadState();};
inline PyFrameObject * PyFrame_GetBack(PyFrameObject * frame) {return ::PyFrame_GetBack(frame);};
inline void PyMem_GetAllocator(PyMemAllocatorDomain domain, PyMemAllocatorEx * alloc) {::PyMem_GetAllocator(domain, alloc);};
inline void PyMem_SetAllocator(PyMemAllocatorDomain domain, PyMemAllocatorEx * alloc) {::PyMem_SetAllocator(domain, alloc);};
inline PyGILState_STATE PyGILState_Ensure(void) {return ::PyGILState_Ensure();};
inline void PyGILState_Release(PyGILState_STATE state) {::PyGILState_Release(state);};
inline void PyEval_SetProfileAllThreads(Py_tracefunc func, PyObject *obj){::PyEval_SetProfileAllThreads(func, obj);};
inline void PyEval_SetTraceAllThreads(Py_tracefunc func, PyObject *obj){::PyEval_SetTraceAllThreads(func, obj);};
inline PyCodeObject * PyFrame_GetCode(PyFrameObject * frame) {return ::PyFrame_GetCode(frame);};
inline PyObject * PyUnicode_AsASCIIString(PyObject * value) {return ::PyUnicode_AsASCIIString(value);};
inline char * PyBytes_AsString(PyObject * obj) {return ::PyBytes_AsString(obj);};
inline int PyFrame_GetLineNumber(PyFrameObject * frame) {return ::PyFrame_GetLineNumber(frame);};
inline void PyObject_Free(void * object) {::PyObject_Free(object);};
inline int Py_BytesMain(int argc, char ** argv) {return ::Py_BytesMain(argc, argv);};
inline void Py_DecRef(PyObject* ptr) {::Py_DECREF(ptr);};
inline Py_ssize_t Py_RefCnt(PyObject* ptr) {return ::Py_REFCNT(ptr);};
inline PyObject * PyUnicode_AsEncodedString(PyObject * value, const char * encoding, const char * mode) {return ::PyUnicode_AsEncodedString(value, encoding, mode);};
inline int Py_IsInitialized(void) {return ::Py_IsInitialized();};

}

#endif //MALT_PYTHON_NATIVE_HPP

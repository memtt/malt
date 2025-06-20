/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 06/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/portability/PythonFake.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_PYTHON_FAKE_HPP
#define MALT_PYTHON_FAKE_HPP

/**********************************************************/
#include <cstdlib>
#include <cstdio>

/**********************************************************/
namespace MALT
{

/**********************************************************/
#define PY_MAJOR_VERSION 3
#define PY_MINOR_VERSION 11

/**********************************************************/
typedef void * (*FakePythonMallocFuncPtr) (void *ctx, size_t size);
typedef void * (*FakePythonCallocFuncPtr) (void *ctx, size_t nelem, size_t elsize);
typedef void * (*FakePythonReallocFuncPtr) (void *ctx, void *ptr, size_t new_size);
typedef void   (*FakePythonFreeFuncPtr) (void *ctx, void *ptr);
typedef ssize_t Py_ssize_t;

/**********************************************************/
typedef bool PyGILState_STATE;

/**********************************************************/
struct PyObject{
	char __unused__;
};
struct PyCodeObject{
	PyObject * co_filename{nullptr};
	PyObject * co_qualname{nullptr};
};
struct PyFrameObject{
	char __unused__;
};
struct PyMemAllocatorEx{
	void * ctx{nullptr};
	void * (*malloc) (void *ctx, size_t size){nullptr};
	void * (*calloc) (void *ctx, size_t nelem, size_t elsize){nullptr};
	void * (*realloc) (void *ctx, void *ptr, size_t new_size){nullptr};
	void   (*free) (void *ctx, void *ptr){nullptr};
};

/**********************************************************/
enum PyTraceEvent
{
	PyTrace_CALL,
	PyTrace_RETURN,
	PyTrace_EXCEPTION,
	PyTrace_LINE,
	PyTrace_C_CALL,
	PyTrace_C_RETURN,
	PyTrace_C_EXCEPTION,
};

/**********************************************************/
enum PyMemDomain
{
	PYMEM_DOMAIN_RAW,
	PYMEM_DOMAIN_OBJ,
	PYMEM_DOMAIN_MEM
};

/**********************************************************/
typedef int (*Py_tracefunc)(PyObject *obj, PyFrameObject *frame, int what, PyObject *arg);

/**********************************************************/
inline bool PyLazyInterfaceInit(void){return false;};

/**********************************************************/
//inline void * _PyThreadState_UncheckedGet(){return nullptr;};
inline PyFrameObject * PyThreadState_GetFrame(PyGILState_STATE gilState){return nullptr;};
inline PyGILState_STATE PyGILState_GetThisThreadState() {return false;};
inline PyFrameObject * PyFrame_GetBack(PyFrameObject * frame) {return nullptr;};
inline void PyMem_GetAllocator(PyMemDomain domain, PyMemAllocatorEx * alloc) {};
inline void PyMem_SetAllocator(PyMemDomain domain, const PyMemAllocatorEx * alloc) {};
inline PyGILState_STATE PyGILState_Ensure(void) {return false;};
inline void PyGILState_Release(PyGILState_STATE state) {};
inline void PyEval_SetProfileAllThreads(Py_tracefunc func, PyObject *obj){};
inline void PyEval_SetTraceAllThreads(Py_tracefunc func, PyObject *obj){};
inline PyCodeObject * PyFrame_GetCode(PyFrameObject * frame) {return nullptr;};
inline PyObject * PyUnicode_AsASCIIString(const char * value) {return nullptr;};
inline char * PyBytes_AsString(PyObject * obj) {return nullptr;};
inline int PyFrame_GetLineNumber(PyFrameObject * frame) {return 0;};
inline void PyObject_Free(void * object) {};
inline int Py_BytesMain(int argc, char ** argv) {fprintf(stderr, "MALT: Built without python support !\n"); return EXIT_FAILURE;};
inline void Py_DecRef(void* ptr) {};
inline PyObject * PyUnicode_AsEncodedString(PyObject * value, const char * encoding, const char * mode) {return nullptr;};
inline Py_ssize_t Py_RefCnt(PyObject* ptr) {return 0;};
inline int Py_IsInitialized(void) {return 0;};

inline PyObject *PySys_GetObject(const char *name) {return nullptr;};
inline PyObject *PyList_GetItem(PyObject *list, Py_ssize_t index){return nullptr;};
inline PyObject *PyImport_ImportModule(const char *name){return nullptr;};

}

#endif //MALT_PYTHON_FAKE_HPP

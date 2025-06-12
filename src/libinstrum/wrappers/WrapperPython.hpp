/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 06/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/wrappers/WrapperPython.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#ifndef MALT_WRAPPER_PYTHON_ALLOC_HPP
#define MALT_WRAPPER_PYTHON_ALLOC_HPP

/**********************************************************/
#include "state/GlobalState.hpp"
#include "portability/Python.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
typedef void * (*PythonMallocFuncPtr) (void *ctx, size_t size);
typedef void * (*PythonCallocFuncPtr) (void *ctx, size_t nelem, size_t elsize);
typedef void * (*PythonReallocFuncPtr) (void *ctx, void *ptr, size_t new_size);
typedef void   (*PythonFreeFuncPtr) (void *ctx, void *ptr);

/**********************************************************/
void * malt_wrap_python_malloc(void * ctx, size_t size, PythonMallocFuncPtr real_malloc, void * retaddr, AllocDomain domain);
void malt_wrap_python_free(void * ctx, void * ptr, PythonFreeFuncPtr real_free, void * retaddr, AllocDomain domain);
void * malt_wrap_python_calloc(void * ctx, size_t nmemb,size_t size, PythonCallocFuncPtr real_calloc, void * retaddr, AllocDomain domain);
void * malt_wrap_python_realloc(void * ctx,void * ptr,size_t size, PythonReallocFuncPtr real_realloc, void * retaddr, AllocDomain domain);

/**********************************************************/
int malt_wrap_python_on_enter_exit(PyObject *obj, PyFrameObject *frame, int what, PyObject *arg);

}

#endif //MALT_WRAPPER_PYTHON_ALLOC_HPP

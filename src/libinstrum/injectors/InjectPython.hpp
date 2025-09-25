/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 06/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/injectors/InjectPython.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_INJECT_PYTHON_ALLOC_HPP
#define MALT_INJECT_PYTHON_ALLOC_HPP

/**********************************************************/
#include <cstdlib>

/**********************************************************/
namespace MALT
{

/**********************************************************/
//raw domain
struct WrapperPythonRaw {
    static void* malloc(void* ctx, size_t size);
    static void free(void* ctx, void* ptr);
    static void* calloc(void* ctx, size_t nelem, size_t elsize);
    static void* realloc(void* ctx, void *ptr, size_t new_size);
};

/**********************************************************/
//mem domain
struct WrapperPythonMem {
    static void* malloc(void* ctx, size_t size);
    static void free(void* ctx, void* ptr);
    static void* calloc(void* ctx, size_t nelem, size_t elsize);
    static void* realloc(void* ctx, void *ptr, size_t new_size);
};

/**********************************************************/
//object domain
struct WrapperPythonObj {
    static void* malloc(void* ctx, size_t size);
    static void free(void* ctx, void* ptr);
    static void* calloc(void* ctx, size_t nelem, size_t elsize);
    static void* realloc(void* ctx, void *ptr, size_t new_size);
};

int pythonOnPorfile(PyObject *obj, PyFrameObject *frame, int what, PyObject *arg);
void initPythonAllocInstrumentation(void);
void initPythonEnterExitInstrumentation(void);
void initPythonInstrumentation(const char * script = nullptr);

}

#endif //MALT_INJECT_PYTHON_ALLOC_HPP

/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.0
*    DATE     : 06/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/injectors/InjectPythonInit.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#ifndef MALT_INJECT_PYTHON_INIT_HPP
#define MALT_INJECT_PYTHON_INIT_HPP

/**********************************************************/
#include <dlfcn.h>
#include "../portability/Python.hpp"
#include "../portability/Visibility.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
//TODO try to put in GlobalState.
extern bool gblHavePython;
void initPythonInstrumentation(const char * script = nullptr);
typedef int (*Py_RunMainFuncPtr) (void);
typedef int (*Py_BytesMainFuncPtr) (int argc, char ** argv);

/**********************************************************/
#ifdef MALT_HAVE_PYTHON
	inline bool havePython(void)
	{
		return gblHavePython;
	}
#else //MALT_HAVE_PYTHON
	inline bool havePython(void)
	{
		return false;
	}	
#endif //MALT_HAVE_PYTHON

/**********************************************************/
inline void initPythonLazy(bool force = false)
{
	static bool done = false;
	if (done == false && havePython() && MALT::Py_IsInitialized()) {
		done = true;
		initPythonInstrumentation();	
	}
}

}

/**********************************************************/
/*#ifdef MALT_HAVE_PYTHON
	DLL_PUBLIC int Py_RunMain()
	{
		MALT::Py_RunMainFuncPtr realPy_RunMain = (MALT::Py_RunMainFuncPtr)dlsym(RTLD_NEXT,"Py_RunMain");
		MALT::initPythonLazy();
		return realPy_RunMain();
	}
#endif //MALT_HAVE_PYTHON*/

/**********************************************************/
/*#ifdef MALT_HAVE_PYTHON
	DLL_PUBLIC int Py_BytesMain(int argc, char **argv)
	{
		MALT::Py_BytesMainFuncPtr realPy_BytesMain = (MALT::Py_BytesMainFuncPtr)dlsym(RTLD_NEXT,"Py_BytesMain");
		fprintf(stderr, "MALT: Py_BytesMain\n");
		Py_Initialize();
		MALT::initPythonLazy();
		return realPy_BytesMain(argc, argv);
	}
#endif //MALT_HAVE_PYTHON*/

#endif //MALT_INJECT_PYTHON_INIT_HPP

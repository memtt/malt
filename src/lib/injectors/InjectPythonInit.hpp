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
#ifndef MALT_INJECT_PYTHON_INIT_HPP
#define MALT_INJECT_PYTHON_INIT_HPP

/**********************************************************/
#include <Python.h>

/**********************************************************/
namespace MALT
{
	void initPythonInstrumentation();
	typedef int (*Py_RunMainFuncPtr) (void);
}

/**********************************************************/
int Py_RunMain()
{
	MALT::Py_RunMainFuncPtr realPy_RunMain = (MALT::Py_RunMainFuncPtr)dlsym(RTLD_NEXT,"Py_RunMain");
	MALT::initPythonInstrumentation();
	return realPy_RunMain();
}

#endif //MALT_INJECT_PYTHON_INIT_HPP

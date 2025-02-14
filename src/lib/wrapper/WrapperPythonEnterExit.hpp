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
#ifndef MALT_WRAPPER_PYTHON_ENTER_EXIT_HPP
#define MALT_WRAPPER_PYTHON_ENTER_EXIT_HPP

/**********************************************************/
#include <Python.h>

/**********************************************************/
namespace MALT
{

int pythonOnEnterExit(PyObject *obj, PyFrameObject *frame, int what, PyObject *arg);

}

#endif //MALT_WRAPPER_PYTHON_ENTER_EXIT_HPP

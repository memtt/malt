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
#include <Python.h>
#include "GlobalState.hpp"
#include "WrapperPythonAlloc.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
int pythonOnEnterExit(PyObject *obj, PyFrameObject *frame, int what, PyObject *arg)
{
	switch (what)
	{
		case PyTrace_CALL:
			printf("Enter call %p\n", frame);
			break;
		case PyTrace_RETURN:
			printf("Return call %p\n", frame);
			break;
		case PyTrace_C_CALL:
			printf("Enter C call %p\n", frame);
			break;
		case PyTrace_C_RETURN:
			printf("Enter C call %p\n", frame);
			break;
		default:{}
			//ignored
	};

	//ok
	return 0;
}

}

/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/integration/malt-python3.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/

#include <cstdio>
#include <Python.h>
//#include <thread>
//libc POSIX.1, here we use GNU specific RTLD_NEXT (need _GNU_SOURCE)
#include <dlfcn.h>
#include "../libinstrum/injectors/InjectPythonInit.hpp"

/**********************************************************/
int main(int argc, char** argv)
{
	return Py_BytesMain(argc, argv);
}

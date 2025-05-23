/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/portability/tests/TestCompiler.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2022 - 2024
***********************************************************/

#include "MangledFunctions.hpp"

/**********************************************************/
int notMangledCFunction(bool p1, int p2)
{
	return p2;
}

/**********************************************************/
int mangledCppFunction(bool p1, int p2)
{
	return p2;
}

/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.0
*    DATE     : 03/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/portability/tests/MangledFunctions.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
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

/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 03/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/portability/tests/MangledFunctions.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#ifndef MALT_TEST_MANGLED_FUNCTIONS_HPP
#define MALT_TEST_MANGLED_FUNCTIONS_HPP

/**********************************************************/
#include <string>

/**********************************************************/
extern "C" {
	int notMangledCFunction(bool p1, int p2);
}

/**********************************************************/
int mangledCppFunction(bool p1, int p2);

#endif //MALT_TEST_MANGLED_FUNCTIONS_HPP

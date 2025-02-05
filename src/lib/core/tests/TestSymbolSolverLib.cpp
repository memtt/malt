/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.5
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/core/tests/TestSymbolSolverLib.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2020 - 2024
***********************************************************/
/********************  HEADERS  *********************/
#include <stacks/BacktraceStack.hpp>

/***************** USING NAMESPACE ******************/
using namespace MALT;

/*******************  FUNCTION  *********************/
void testCalleeLib(BacktraceStack & stack)
{
	stack.loadCurrentStack();
}

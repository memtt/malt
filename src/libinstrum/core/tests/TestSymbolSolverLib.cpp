/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 03/2020
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/core/tests/TestSymbolSolverLib.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2020
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

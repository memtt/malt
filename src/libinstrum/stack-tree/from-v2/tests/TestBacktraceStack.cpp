/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2018
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/stack-tree/from-v2/tests/TestBacktraceStack.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2015 - 2018
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include <stacks/BacktraceStack.hpp>
#include <common/SimpleAllocator.hpp>
#include <common/Options.hpp>

/**********************************************************/
using namespace MALT;

/**********************************************************/
TEST(Stack,constructorAndLoadCurrentStack)
{
	BacktraceStack stack;
	stack.loadCurrentStack();
}

/**********************************************************/
int main(int argc, char ** argv)
{
	//init internal allocator
	initInternalAlloc();
	MALT::initGlobalOptions();
	
	// This allows the user to override the flag on the command line.
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

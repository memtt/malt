/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.5
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/stacks/tests/TestBacktraceStack.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
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
	gblInternaAlloc = new SimpleAllocator(true);
	gblOptions = new Options;
	
	// This allows the user to override the flag on the command line.
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

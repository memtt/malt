/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 09/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/stacks/tests/TestEnterExitStack.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include <EnterExitStack.hpp>
#include <common/SimpleAllocator.hpp>

/**********************************************************/
using namespace MALT;

/**********************************************************/
TEST(Stack,test1)
{
	EnterExitStack stack;
	
	for (int i = 0 ; i < 6 ; i++)
		stack.enterFunction((void*)(size_t)i);
	
	stack.exitFunction((void*)0x5);
	
	for (int i = 0 ; i < 5 ; i++)
		EXPECT_EQ((void*)(size_t)(4-i),stack[i]);
}

/**********************************************************/
int main(int argc, char ** argv)
{
	//init internal allocator
	gblInternaAlloc = new SimpleAllocator(true);
	
	// This allows the user to override the flag on the command line.
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

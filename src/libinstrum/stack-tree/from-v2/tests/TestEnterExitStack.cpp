/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/stack-tree/from-v2/tests/TestEnterExitStack.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2015 - 2018
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include <EnterExitStack.hpp>
#include <common/SimpleAllocator.hpp>

/**********************************************************/
using namespace MALTV2;

/**********************************************************/
TEST(Stack,test1)
{
	EnterExitStack stack;
	
	for (int i = 0 ; i < 6 ; i++)
		stack.enterFunction((void*)(size_t)i);
	
	stack.exitFunction((void*)0x5);
	
	for (int i = 0 ; i < 5 ; i++)
		EXPECT_EQ(MALT::LangAddress(MALT::DOMAIN_C, (void*)(size_t)(4-i)),stack[i]);
}

/**********************************************************/
int main(int argc, char ** argv)
{
	//init internal allocator
	MALT::initInternalAlloc();
	
	// This allows the user to override the flag on the command line.
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

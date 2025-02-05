/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <EnterExitStack.hpp>
#include <common/SimpleAllocator.hpp>

/***************** USING NAMESPACE ******************/
using namespace MALT;

/*******************  FUNCTION  *********************/
TEST(Stack,test1)
{
	EnterExitStack stack;
	
	for (int i = 0 ; i < 6 ; i++)
		stack.enterFunction((void*)(size_t)i);
	
	stack.exitFunction((void*)0x5);
	
	for (int i = 0 ; i < 5 ; i++)
		EXPECT_EQ(AddressType(DOMAIN_C, (void*) (size_t)(4-i)),stack[i]);
}

/*******************  FUNCTION  *********************/
int main(int argc, char ** argv)
{
	//init internal allocator
	gblInternaAlloc = new SimpleAllocator(true);
	
	// This allows the user to override the flag on the command line.
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

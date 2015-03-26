/*****************************************************
             PROJECT  : MALT
             VERSION  : 0.1.0
             DATE     : 03/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <stacks/BacktraceStack.hpp>
#include <common/SimpleAllocator.hpp>

/***************** USING NAMESPACE ******************/
using namespace MALT;

/*******************  FUNCTION  *********************/
TEST(Stack,constructorAndLoadCurrentStack)
{
	BacktraceStack stack;
	stack.loadCurrentStack();
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

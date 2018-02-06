/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.0.0
             DATE     : 02/2018
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <stacks/BacktraceStack.hpp>
#include <common/SimpleAllocator.hpp>
#include <common/Options.hpp>

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
	gblOptions = new Options;
	
	// This allows the user to override the flag on the command line.
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

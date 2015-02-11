/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <allocators/SimpleAllocator.hpp>
#include <gtest/gtest.h>

/*******************  FUNCTION  *********************/
int main(int argc, char ** argv)
{
	//init internal allocator
	MATT::gblInternaAlloc = new MATT::SimpleAllocator(true);
	
	// This allows the user to override the flag on the command line.
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
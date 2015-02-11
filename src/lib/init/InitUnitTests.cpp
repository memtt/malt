/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <allocators/SimpleAllocator.hpp>
#include <allocators/NoFreeAllocator.hpp>
#include <core/Options.hpp>
#include <gtest/gtest.h>

/*******************  FUNCTION  *********************/
int main(int argc, char ** argv)
{
	//init internal allocator
	MATT::doNoFreeAllocatorInit();
	void * ptr = MATT::gblNoFreeAllocator.allocate(sizeof(MATT::SimpleAllocator));
	MATT::gblInternaAlloc = new (ptr) MATT::SimpleAllocator(true);
	MATT::initGlobalOptions();
	
	// This allows the user to override the flag on the command line.
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
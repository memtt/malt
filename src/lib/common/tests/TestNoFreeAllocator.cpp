/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <sstream>
#include <gtest/gtest.h>
#include "NoFreeAllocator.hpp"

/***************** USING NAMESPACE ******************/
using namespace MALTV2;

/*******************  FUNCTION  *********************/
TEST(TestNoFreeAllocator,constructor)
{
	NoFreeAllocator alloc;
	alloc.init();
	ASSERT_EQ(2097120, alloc.getMaxSize());
}

/*******************  FUNCTION  *********************/
TEST(TestNoFreeAllocator,allocate)
{
	NoFreeAllocator alloc;
	alloc.init();
	void * ptr1 = alloc.allocate(128);
	void * ptr2 = alloc.allocate(128);
	ASSERT_NE(ptr1, ptr2);
	memset(ptr1, 0, 128);
	memset(ptr2, 0, 128);
}

/*******************  FUNCTION  *********************/
TEST(TestNoFreeAllocator,allocate_mmap)
{
	NoFreeAllocator alloc;
	alloc.init(true, true);
	void * ptr1 = alloc.allocate(128);
	void * ptr2 = alloc.allocate(128);
	ASSERT_NE(ptr1, ptr2);
	memset(ptr1, 0, 128);
	memset(ptr2, 0, 128);
}

/*******************  FUNCTION  *********************/
TEST(TestNoFreeAllocator,stats)
{
	NoFreeAllocator alloc;
	alloc.init();
	alloc.allocate(128);
	alloc.allocate(128);
	EXPECT_EQ(288, alloc.getInuseMemory());
	EXPECT_EQ(2097152 - 288, alloc.getUnusedMemory());
	EXPECT_EQ(2097152, alloc.getTotalMemory());
}

/*******************  FUNCTION  *********************/
TEST(TestNoFreeAllocator,printState)
{
	NoFreeAllocator alloc;
	alloc.init();
	alloc.allocate(128);
	alloc.allocate(128);
	std::stringstream out;
	alloc.printState(out);
	EXPECT_EQ(out.str(),
		"=============================================================== MATT MEMORY =====================================================================\n"
		"Internal memory : allocated =   2.1 MB , unused =   2.1 MB\n"
		"=================================================================================================================================================\n"
	);
}

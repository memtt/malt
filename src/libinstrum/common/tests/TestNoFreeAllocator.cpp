/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 01/2022
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/common/tests/TestNoFreeAllocator.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2022
***********************************************************/

/**********************************************************/
#include <sstream>
#include <gtest/gtest.h>
#include "NoFreeAllocator.hpp"

/**********************************************************/
using namespace MALTV2;

/**********************************************************/
TEST(TestNoFreeAllocator,constructor)
{
	NoFreeAllocator alloc;
	alloc.init();
	ASSERT_EQ(2097120, alloc.getMaxSize());
}

/**********************************************************/
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

/**********************************************************/
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

/**********************************************************/
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

/**********************************************************/
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

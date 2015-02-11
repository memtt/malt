/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <allocators/NoFreeAllocator.hpp>

/*********************  CONSTS  *********************/
static const char * CST_STRING_1 = "=============================================================== MATT MEMORY =====================================================================\n\
Internal memory : allocated =   2.0 Mo , unused =   2.0 Mo\n\
=================================================================================================================================================\n";


/***************** USING NAMESPACE ******************/
using namespace MATT;

/*******************  FUNCTION  *********************/
TEST(NoFreeAllocator,constructor)
{
	NoFreeAllocator allocator;
	allocator.init();
	allocator.allocate(16);
}

/*******************  FUNCTION  *********************/
TEST(NoFreeAllocator,getMaxSize)
{
	NoFreeAllocator allocator;
	allocator.init();
	EXPECT_EQ(2*1024*1024-4*sizeof(void*),allocator.getMaxSize());
}

/*******************  FUNCTION  *********************/
TEST(NoFreeAllocator,allocate)
{
	NoFreeAllocator allocator;
	allocator.init();
	char * ptr1 = (char*)allocator.allocate(16);
	char * ptr2 = (char*)allocator.allocate(16);
	char * ptr3 = (char*)allocator.allocate(16);
	EXPECT_EQ(16,ptr2-ptr1);
	EXPECT_EQ(16,ptr3-ptr2);
}

/*******************  FUNCTION  *********************/
TEST(NoFreeAllocator,stats)
{
	NoFreeAllocator allocator;
	allocator.init();
	allocator.allocate(16);
	EXPECT_EQ(16+sizeof(NoFreeAllocatorSegment),allocator.getInuseMemory());
	EXPECT_EQ(NO_FREE_ALLOC_SEG_SIZE,allocator.getTotalMemory());
	EXPECT_EQ(NO_FREE_ALLOC_SEG_SIZE-16-sizeof(NoFreeAllocatorSegment),allocator.getUnusedMemory());
}

/*******************  FUNCTION  *********************/
TEST(NoFreeAllocator,printState)
{
	std::stringstream out;
	NoFreeAllocator allocator;
	allocator.init();
	allocator.allocate(16);
	allocator.printState(out);
	EXPECT_EQ(CST_STRING_1,out.str());
}

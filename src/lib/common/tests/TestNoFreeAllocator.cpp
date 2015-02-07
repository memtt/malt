/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include "NoFreeAllocator.hpp"

/***************** USING NAMESPACE ******************/
using namespace MATT;

/*******************  FUNCTION  *********************/
TEST(NoFreeAllocator,constructor)
{
	NoFreeAllocator allocator;
}

/*******************  FUNCTION  *********************/
TEST(NoFreeAllocator,memory)
{
	NoFreeAllocator allocator;
	EXPECT_EQ(2*1024*1024,allocator.memory());
}

/*******************  FUNCTION  *********************/
TEST(NoFreeAllocator,allocate)
{
	NoFreeAllocator allocator;
	char * ptr1 = (char*)allocator.allocate(16);
	char * ptr2 = (char*)allocator.allocate(16);
	char * ptr3 = (char*)allocator.allocate(16);
	EXPECT_EQ(16,ptr2-ptr1);
	EXPECT_EQ(16,ptr3-ptr2);
}

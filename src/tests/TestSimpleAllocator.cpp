/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <common/SimpleAllocator.hpp>

/***************** USING NAMESPACE ******************/
using namespace MATT;

/*******************  FUNCTION  *********************/
TEST(SimpleAllocator,constructor)
{
	SimpleAllocator alloc;
}

/*******************  FUNCTION  *********************/
TEST(SimpleAllocator,alloc1)
{
	SimpleAllocator alloc;
	void * ptr = alloc.malloc(16);
	EXPECT_NE((void*)NULL,ptr);
	alloc.free(ptr);
}

/*******************  FUNCTION  *********************/
TEST(SimpleAllocator,alloc2)
{
	SimpleAllocator alloc;
	void * ptr1 = alloc.malloc(16);
	alloc.free(ptr1);
	void * ptr2 = alloc.malloc(16);
	EXPECT_EQ(ptr1,ptr2);
}

/*******************  FUNCTION  *********************/
TEST(SimpleAllocator,alloc3)
{
	SimpleAllocator alloc;
	void * ptr1 = alloc.malloc(16);
	void * ptr2 = alloc.malloc(16);
	void * ptr3 = alloc.malloc(16);
	EXPECT_NE(ptr1,ptr2);
	EXPECT_NE(ptr1,ptr3);
	EXPECT_NE(ptr2,ptr3);
}

/*******************  FUNCTION  *********************/
TEST(SimpleAllocator,alloc4)
{
	SimpleAllocator alloc;
	void * ptr[2048];
	for (int i = 0 ; i < 2048 ; i++)
	{
		ptr[i] = alloc.malloc(64);
		memset(ptr[i],0,64);
	}
	for (int i = 0 ; i < 2048 ; i++)
		alloc.free(ptr[i]);
}

/*******************  FUNCTION  *********************/
TEST(SimpleAllocator,alloc5)
{
	SimpleAllocator alloc;
	void * ptr1 = alloc.malloc(16);
	alloc.free(ptr1);
	void * ptr2 = alloc.malloc(64);
	EXPECT_NE(ptr1,ptr2);
}

/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <VmaTracker.hpp>
#include <common/SimpleAllocator.hpp>
#include <common/GTestHelper.hpp>

/***************** USING NAMESPACE ******************/
using namespace MALT;

/*******************  FUNCTION  *********************/
TEST(TestVmaTracker,testConstructor)
{
	VmaTracker tracker;
}

/*******************  FUNCTION  *********************/
TEST(TestVmaTracker,testMmap)
{
	VmaTracker tracker;
	tracker.mmap((void*)0xA00,100);
}

/*******************  FUNCTION  *********************/
TEST(TestVmaTracker,testMunmap_1)
{
	VmaTracker tracker;
	tracker.mmap((void*)0xA00,100);
	EXPECT_EQ(0u,tracker.munmap((void*)0xAA,10));
}

/*******************  FUNCTION  *********************/
TEST(TestVmaTracker,testMunmap_2)
{
	VmaTracker tracker;
	tracker.mmap((void*)0xA00,100);
	EXPECT_EQ(100u,tracker.munmap((void*)0xA00,100));
}

/*******************  FUNCTION  *********************/
TEST(TestVmaTracker,testMunmap_3)
{
	VmaTracker tracker;
	tracker.mmap((void*)0xA00,100);
	EXPECT_EQ(90u,tracker.munmap((void*)0xA0A,100));
	EXPECT_EQ(10u,tracker.munmap((void*)0xA00,100));
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

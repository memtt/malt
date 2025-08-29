/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/core/tests/TestVmaTracker.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include <VmaTracker.hpp>
#include <common/SimpleAllocator.hpp>
#include <common/GTestHelper.hpp>

/**********************************************************/
using namespace MALT;

/**********************************************************/
TEST(TestVmaTracker,testConstructor)
{
	VmaTracker tracker;
}

/**********************************************************/
TEST(TestVmaTracker,testMmap)
{
	VmaTracker tracker;
	EXPECT_EQ(100, tracker.mmap((void*)0xA00,100));
}

/**********************************************************/
TEST(TestVmaTracker,testMunmap_1)
{
	VmaTracker tracker;
	EXPECT_EQ(100, tracker.mmap((void*)0xA00,100));
	EXPECT_EQ(0,tracker.munmap((void*)0xAA,10));
}

/**********************************************************/
TEST(TestVmaTracker,testMunmap_2)
{
	VmaTracker tracker;
	EXPECT_EQ(100, tracker.mmap((void*)0xA00,100));
	EXPECT_EQ(-100,tracker.munmap((void*)0xA00,100));
}

/**********************************************************/
TEST(TestVmaTracker,testMunmap_3)
{
	VmaTracker tracker;
	EXPECT_EQ(100, tracker.mmap((void*)0xA00,100));
	EXPECT_EQ(-90,tracker.munmap((void*)0xA0A,100));
	EXPECT_EQ(-10,tracker.munmap((void*)0xA00,100));
}

/**********************************************************/
TEST(TestVmaTracker,operatorOutputStream)
{
	VmaTracker tracker;
	ASSERT_EQ(100, tracker.mmap((void*)0xA00,100));
	ASSERT_EQ(-100, tracker.munmap((void*)0xA00,100));
	ASSERT_EQ(100, tracker.mmap((void*)0xA00,100));
	ASSERT_EQ(200, tracker.mmap((void*)0xE00,200));
	ASSERT_EQ(200, tracker.mmap((void*)0xE0000,200));

	std::stringstream out;
	out << tracker;

	ASSERT_EQ(out.str(), "[0 - 0 / 0]\n[2560 - 2660 / 100]\n[3584 - 3784 / 200]\n");
}

/**********************************************************/
TEST(TestVmaTracker,compact_ok)
{
	VmaTracker tracker;
	ASSERT_EQ(100, tracker.mmap((void*)0xA00,100));
	ASSERT_EQ(-100, tracker.munmap((void*)0xA00,100));
	for (size_t i = 0 ; i < 1500 ; i++)
		ASSERT_EQ(4096, tracker.mmap((void*)(4096 + 4096 * i),4096));
	EXPECT_EQ(tracker.getCount(), 1);
	EXPECT_EQ(tracker.getInsertPosition(), 1);
}

/**********************************************************/
TEST(TestVmaTracker,compact_holes)
{
	VmaTracker tracker;
	ASSERT_EQ(100, tracker.mmap((void*)0xA00,100));
	ASSERT_EQ(-100, tracker.munmap((void*)0xA00,100));
	for (size_t i = 0 ; i < 1500 ; i++)
		ASSERT_EQ(100, tracker.mmap((void*)(4096 + 4096 * i),100));
	EXPECT_EQ(tracker.getCount(), 1500);
	EXPECT_EQ(tracker.getInsertPosition(), 1500);
}

/**********************************************************/
int main(int argc, char ** argv)
{
	//init internal allocator
	gblInternaAlloc = new SimpleAllocator(true);
	
	// This allows the user to override the flag on the command line.
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

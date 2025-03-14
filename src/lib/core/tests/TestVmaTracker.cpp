/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 09/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/core/tests/TestVmaTracker.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
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
	tracker.mmap((void*)0xA00,100);
}

/**********************************************************/
TEST(TestVmaTracker,testMunmap_1)
{
	VmaTracker tracker;
	tracker.mmap((void*)0xA00,100);
	EXPECT_EQ(0u,tracker.munmap((void*)0xAA,10));
}

/**********************************************************/
TEST(TestVmaTracker,testMunmap_2)
{
	VmaTracker tracker;
	tracker.mmap((void*)0xA00,100);
	EXPECT_EQ(100u,tracker.munmap((void*)0xA00,100));
}

/**********************************************************/
TEST(TestVmaTracker,testMunmap_3)
{
	VmaTracker tracker;
	tracker.mmap((void*)0xA00,100);
	EXPECT_EQ(90u,tracker.munmap((void*)0xA0A,100));
	EXPECT_EQ(10u,tracker.munmap((void*)0xA00,100));
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

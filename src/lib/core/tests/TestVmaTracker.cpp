/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
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
TEST(TestVmaTracker,operatorOutputStream)
{
	VmaTracker tracker;
	tracker.mmap((void*)0xA00,100);
	tracker.munmap((void*)0xA00,100);
	tracker.mmap((void*)0xA00,100);
	tracker.mmap((void*)0xE00,200);
	tracker.mmap((void*)0xE0000,200);


	std::stringstream out;
	out << tracker;

	ASSERT_EQ(out.str(), "[0 - 0]\n[2560 - 2660]\n[3584 - 3784]\n");
}

/**********************************************************/
/*TEST(TestVmaTracker,compact)
{
	VmaTracker tracker;
	tracker.mmap((void*)0xA00,100);
	tracker.munmap((void*)0xA00,100);
	for (size_t i = 0 ; i < 1500 ; i++)
		tracker.mmap((void*)(4096 + 4096 * i),100);
	EXPECT_EQ(tracker.getCount(), 1500);
	EXPECT_EQ(tracker.getInsertPosition(), 1500);
}*/

/**********************************************************/
int main(int argc, char ** argv)
{
	//init internal allocator
	gblInternaAlloc = new SimpleAllocator(true);
	
	// This allows the user to override the flag on the command line.
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

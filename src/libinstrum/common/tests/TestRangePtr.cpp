/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.0
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/common/tests/TestRangePtr.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include "../RangePtr.hpp"

/**********************************************************/
using namespace MALT;

/**********************************************************/
TEST(TestRangePtr, constructor)
{
	RangePtr range1((void*)0x100, (void*)0x300);
	ASSERT_FALSE(range1.contains((void*)0x99));
	ASSERT_TRUE(range1.contains((void*)0x100));
	ASSERT_TRUE(range1.contains((void*)0x200));
	ASSERT_TRUE(range1.contains((void*)0x299));
	ASSERT_FALSE(range1.contains((void*)0x300));

	RangePtr range2((void*)0x100, 0x200);
	ASSERT_FALSE(range2.contains((void*)0x99));
	ASSERT_TRUE(range2.contains((void*)0x100));
	ASSERT_TRUE(range2.contains((void*)0x200));
	ASSERT_TRUE(range2.contains((void*)0x299));
	ASSERT_FALSE(range2.contains((void*)0x300));
}

/**********************************************************/
TEST(TestRangePtr, null)
{
	RangePtr range1(nullptr, nullptr);
	ASSERT_FALSE(range1.contains((void*)0x99));
	ASSERT_FALSE(range1.contains((void*)0x100));
	ASSERT_FALSE(range1.contains((void*)0x200));
	ASSERT_FALSE(range1.contains((void*)0x299));
	ASSERT_FALSE(range1.contains((void*)0x300));
}

/**********************************************************/
TEST(TestRangePtr, set)
{
	RangePtr range1(nullptr, nullptr);
	range1.set((void*)0x100, (void*)0x300);
	ASSERT_FALSE(range1.contains((void*)0x99));
	ASSERT_TRUE(range1.contains((void*)0x100));
	ASSERT_TRUE(range1.contains((void*)0x200));
	ASSERT_TRUE(range1.contains((void*)0x299));
	ASSERT_FALSE(range1.contains((void*)0x300));

	RangePtr range2(nullptr, nullptr);
	range2.set((void*)0x100, 0x200);
	ASSERT_FALSE(range2.contains((void*)0x99));
	ASSERT_TRUE(range2.contains((void*)0x100));
	ASSERT_TRUE(range2.contains((void*)0x200));
	ASSERT_TRUE(range2.contains((void*)0x299));
	ASSERT_FALSE(range2.contains((void*)0x300));
}

/**********************************************************/
TEST(TestRangesPtr, set)
{
	RangesPtr ranges;
	ranges.add((void*)0x100, 0x200);
	ranges.add((void*)0x1000, 0x200);

	ASSERT_FALSE(ranges.contains((void*)0x99));
	ASSERT_TRUE(ranges.contains((void*)0x100));
	ASSERT_FALSE(ranges.contains((void*)0x300));
	ASSERT_TRUE(ranges.contains((void*)0x1199));
	ASSERT_FALSE(ranges.contains((void*)0x1300));
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

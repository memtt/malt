/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/callgraph/tests/TestSimpleIdCache.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include "../SimpleIdCache.hpp"

/**********************************************************/
using namespace MALTReader;

/**********************************************************/
TEST(TestSimpleIdCache, constructor)
{
	SimpleIdCache cache;
}

/**********************************************************/
TEST(TestSimpleIdCache, exists)
{
	SimpleIdCache cache;
	ASSERT_FALSE(cache.exists("test"));
	cache.put("test");
	ASSERT_TRUE(cache.exists("test"));
}

/**********************************************************/
TEST(TestSimpleIdCache, put)
{
	SimpleIdCache cache;
	ASSERT_EQ(1,cache.put("test"));
	ASSERT_EQ(1,cache.put("test"));
	ASSERT_EQ(2,cache.put("test2"));
	ASSERT_EQ(3,cache.put("test3"));
	ASSERT_EQ(2,cache.put("test2"));
}

/**********************************************************/
TEST(TestSimpleIdCache, get)
{
	SimpleIdCache cache;
	ASSERT_EQ(1,cache.put("test"));
	ASSERT_EQ(2,cache.put("test2"));
	ASSERT_EQ(3,cache.put("test3"));
	
	ASSERT_EQ(1,cache.get("test"));
	ASSERT_EQ(2,cache.get("test2"));
	ASSERT_EQ(3,cache.get("test3"));
	ASSERT_EQ(-1,cache.get("test4"));
}

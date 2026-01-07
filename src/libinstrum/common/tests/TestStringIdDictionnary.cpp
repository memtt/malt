/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 09/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/common/tests/TestStringIdDictionnary.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include "../StringIdDictionnary.hpp"

/**********************************************************/
using namespace MALT;

/**********************************************************/
TEST(TestStringIdDictionnary, constructor)
{
	StringIdDictionnary dict;
}

/**********************************************************/
TEST(TestStringIdDictionnary, flow)
{
	StringIdDictionnary dict;
	int id1 = dict.getId("stringA");
	int id2 = dict.getId("stringB");
	ASSERT_EQ(id1, 0);
	ASSERT_EQ(id2, 1);
	ASSERT_EQ(dict.getString(id1), "stringA");
	ASSERT_EQ(dict.getString(id2), "stringB");
}

/**********************************************************/
TEST(TestStringIdDictionnary, flow_cache)
{
	StringIdDictionnary dict;
	int id1 = dict.getId("stringA");
	int id2 = dict.getId("stringB");
	ASSERT_EQ(id1, 0);
	ASSERT_EQ(id2, 1);
	ASSERT_EQ(dict.getString(id1), "stringA");
	ASSERT_EQ(dict.getString(id2), "stringB");

	//use cache
	id1 = dict.getId("stringA");
	id2 = dict.getId("stringB");
	ASSERT_EQ(id1, 0);
	ASSERT_EQ(id2, 1);
	ASSERT_EQ(dict.getString(id1), "stringA");
	ASSERT_EQ(dict.getString(id2), "stringB");
}

/**********************************************************/
TEST(TestStringIdDictionnary, flow_cache_full)
{
	StringIdDictionnary dict;
	for (size_t i = 0 ; i < 10000 ; i++) {
		char buffer[4096];
		sprintf(buffer, "string-%zu", i);
		int id1 = dict.getId(buffer);
		int id2 = dict.getId(buffer);
		ASSERT_EQ(id1, id2);
		ASSERT_EQ(dict.getString(id2), buffer);
	}
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

/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 02/2025
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
	ASSERT_EQ(dict.getString(id1), "stringA");
	ASSERT_EQ(dict.getString(id2), "stringB");
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

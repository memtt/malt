/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 03/2025
*    LICENSE  : CeCILL-C
*    FILE     : ./src/reader/libreader/json/tests/TestJson.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include "../Json.hpp"

/**********************************************************/
using namespace MALTJson;

/**********************************************************/
TEST(TestJson, iterate_array)
{
	JsonRoot root = parse("[10, 20, 30, 40]");
	ASSERT_EQ(root[0], 10);
	ASSERT_EQ(root[1], 20);
	ASSERT_EQ(root[2], 30);
	ASSERT_EQ(root[3], 40);
}

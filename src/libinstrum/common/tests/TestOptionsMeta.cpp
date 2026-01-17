/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2022
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/common/tests/TestNoFreeAllocator.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2022
***********************************************************/

/**********************************************************/
#include <sstream>
#include <gtest/gtest.h>
#include "../OptionsMeta.hpp"

/**********************************************************/
using namespace MALT;

/**********************************************************/
TEST(TestOptionsMeta,constructor)
{
	bool value{true};
	OptionMeta option("stack", "enable", value);
	ASSERT_EQ("stack", option.getGroup());
	ASSERT_EQ("enable", option.getKey());
	ASSERT_EQ("stack:enable", option.getGroupKey());
}

/**********************************************************/
TEST(TestOptionsMeta,set)
{
	bool value{true};
	OptionMeta option("stack", "enable", value);
	option.setDoc("This is a boolean");
	ASSERT_TRUE(value);
	ASSERT_EQ("This is a boolean", option.getDoc());
}

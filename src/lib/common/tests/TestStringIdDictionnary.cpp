/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/common/Helpers.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
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

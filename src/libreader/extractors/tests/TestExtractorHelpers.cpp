/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/core/tests/TestAllocTraceFile.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2022 - 2024
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include "../ExtractorHelpers.hpp"

/**********************************************************/
using namespace MALTReader;

/**********************************************************/
TEST(TestExtractorHelpers, isAllocFunction)
{
	ASSERT_FALSE(ExtractorHelpers::isAllocFunction("main"));
	ASSERT_TRUE(ExtractorHelpers::isAllocFunction("malloc"));
	ASSERT_FALSE(ExtractorHelpers::isAllocFunction("mmalloc"));
}

/**********************************************************/
TEST(TestExtractorHelpers, jsonCheckPath)
{
	const std::vector<std::string> fieldsToKeep{"ROOT.[].min", "*.max"};
	
	std::vector<std::string> path1{"ROOT", "[]", "min"};
	std::vector<std::string> path2{"ROOT", "[]", "max"};
	std::vector<std::string> path3{"ROOT", "[]", "mean"};

	EXPECT_TRUE(ExtractorHelpers::jsonCheckPath(fieldsToKeep, path1));
	EXPECT_TRUE(ExtractorHelpers::jsonCheckPath(fieldsToKeep, path2));
	EXPECT_FALSE(ExtractorHelpers::jsonCheckPath(fieldsToKeep, path3));
}

/**********************************************************/
TEST(TestExtractorHelpers, filterJson)
{
	const std::vector<std::string> fieldsToKeep{"ROOT.[].min", "*.max"};
	
	const std::string jsonStr = "[{\"min\":10, \"max\":20, \"mean\":30}, {\"min\":15, \"max\":25, \"mean\":35}]";
	const std::string jsonStrRef = "[{\"min\":10, \"max\":20}, {\"min\":15, \"max\":25}]";
	nlohmann::json json = nlohmann::json::parse(jsonStr);
	nlohmann::json jsonRef = nlohmann::json::parse(jsonStrRef);

	ExtractorHelpers::filterJson(json, fieldsToKeep);

	EXPECT_EQ(json, jsonRef);
}

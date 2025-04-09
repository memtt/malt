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

/**********************************************************/
TEST(TestExtractorHelpers, jsonRemoveAbsPath)
{
	const std::string jsonStr = "[{\"path\": \"/tmp/malt/bin/malt\", \"min\":10, \"max\":20}, [\"/tmp/malt/bin/malt\", \"/tmp/malt/bin/malt\", 15]]";
	const std::string jsonStrRef = "[{\"path\": \"bin/malt\", \"min\":10, \"max\":20}, [\"bin/malt\", \"bin/malt\", 15]]";
	nlohmann::json json = nlohmann::json::parse(jsonStr);
	nlohmann::json jsonRef = nlohmann::json::parse(jsonStrRef);

	ExtractorHelpers::jsonRemoveAbsPath(json, "/tmp/malt/");

	EXPECT_EQ(json, jsonRef);
}

/**********************************************************/
TEST(TestExtractorHelpers, jsonRemoveZeroes)
{
	const std::vector<std::string> fieldsToKeep{"ROOT.[].min", "*.max"};
	
	const std::string jsonStr = "[{\"min\":10, \"max\":20, \"mean\":0}, {\"min\":0, \"max\":0, \"mean\":0}, {\"key\": {\"zero\":0}}]";
	const std::string jsonStrRef = "[{\"min\":10, \"max\":20}]";
	nlohmann::json json = nlohmann::json::parse(jsonStr);
	nlohmann::json jsonRef = nlohmann::json::parse(jsonStrRef);

	ExtractorHelpers::jsonRemoveZeroes(json);

	EXPECT_EQ(json, jsonRef);
}

/**********************************************************/
TEST(TestExtractorHelpers, humanReadable)
{
	ASSERT_EQ(std::string("1  B"), ExtractorHelpers::humanReadable(1, 2, "B"));
	ASSERT_EQ(std::string("2.00 KB"), ExtractorHelpers::humanReadable(2048, 2, "B"));
	ASSERT_EQ(std::string("200.00 KB"), ExtractorHelpers::humanReadable(204800, 2, "B"));
	ASSERT_EQ(std::string("3.00 MB"), ExtractorHelpers::humanReadable(3072*1024, 2, "B"));
	ASSERT_EQ(std::string("4.00 GB"), ExtractorHelpers::humanReadable(4096UL*1024UL*1024UL, 2, "B"));
	ASSERT_EQ(std::string("4.00 TB"), ExtractorHelpers::humanReadable(4096UL*1024UL*1024UL*1024UL, 2, "B"));
}
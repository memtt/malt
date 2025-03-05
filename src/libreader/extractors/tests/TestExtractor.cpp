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
#include <fstream>
#include "../Extractor.hpp"
#include "../ExtractorHelpers.hpp"

/**********************************************************/
using namespace MALTFormat;
using namespace MALTReader;

/**********************************************************/
TEST(TestExtractorHelpers, constructor)
{
	MaltProfile profile;
	Extractor extractor(profile);
}

/**********************************************************/
TEST(TestExtractorHelpers, getFlatProfile)
{
	//load
	std::ifstream example(CUR_SRC_DIR "/example.json");
	nlohmann::json data = nlohmann::json::parse(example);
	MaltProfile profile = data;

	//extract
	Extractor extractor(profile);
	FlatProfileVector res = extractor.getFlatProfile([](const InstructionInfosStrRef & location, const MALTFormat::StackInfos & infos){
		return *location.function;
	},[](const InstructionInfosStrRef & location, const MALTFormat::StackInfos & infos){
		return location.file->find("/example.cpp") != location.file->npos;
	});

	//load ref
	std::ifstream exampleExpected(CUR_SRC_DIR "/example.expected.json");
	nlohmann::json dataExpected = nlohmann::json::parse(exampleExpected);

	//remove abs path
	nlohmann::json resJson = ExtractorHelpers::toJsonFiltered(res, {"*.count", "*.location"});
	ExtractorHelpers::jsonRemoveAbsPath(resJson, SRC_DIR "/");

	//check
	EXPECT_EQ(dataExpected["getFlatProfile"], resJson);
	ASSERT_EQ(nlohmann::json::diff(dataExpected["getFlatProfile"], resJson), nlohmann::json::array());
}

/**********************************************************/
TEST(TestExtractorHelpers, getFlatProfile_full)
{
	//load
	std::ifstream example(CUR_SRC_DIR "/example.json");
	nlohmann::json data = nlohmann::json::parse(example);
	MaltProfile profile = data;

	//extract
	Extractor extractor(profile);
	FlatProfileVector res = extractor.getFlatProfile([](const InstructionInfosStrRef & location, const MALTFormat::StackInfos & infos){
		return *location.function;
	},[](const InstructionInfosStrRef & location, const MALTFormat::StackInfos & infos){
		return location.file->find("/example.cpp") != location.file->npos;
	});

	//load ref
	std::ifstream exampleExpected(CUR_SRC_DIR "/example.expected.json");
	nlohmann::json dataExpected = nlohmann::json::parse(exampleExpected);

	//remove abs path
	nlohmann::json resJson = res;
	ExtractorHelpers::jsonRemoveAbsPath(resJson, SRC_DIR "/");

	//check
	ASSERT_EQ(nlohmann::json::diff(dataExpected["getFlatProfile_full"], resJson), nlohmann::json::array());
}

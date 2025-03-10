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
#include <omp.h>
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
	JsonFileIn JsonFileIn(CUR_SRC_DIR "/example.json");
	JsonIn data = JsonFileIn.getRoot();
	MaltProfile profile;
	data.get_to(profile);

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

	//check
	EXPECT_EQ(dataExpected["getFlatProfile"], resJson);
	ASSERT_EQ(nlohmann::json::diff(dataExpected["getFlatProfile"], resJson), nlohmann::json::array());
}

/**********************************************************/
TEST(TestExtractorHelpers, getFlatProfile_full)
{
	//load
	JsonFileIn JsonFileIn(CUR_SRC_DIR "/example.json");
	JsonIn data = JsonFileIn.getRoot();
	MaltProfile profile;
	data.get_to(profile);

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

	//check
	ASSERT_EQ(nlohmann::json::diff(dataExpected["getFlatProfile_full"], resJson), nlohmann::json::array());
}

/**********************************************************/
TEST(TestExtractorHelpers, getProcMapDistr)
{
	//load
	JsonFileIn JsonFileIn(CUR_SRC_DIR "/example.json");
	JsonIn data = JsonFileIn.getRoot();
	MaltProfile profile;
	data.get_to(profile);

	//extract
	Extractor extractor(profile);
	ProcMapDistr res = extractor.getProcMapDistr();

	//load ref
	std::ifstream exampleExpected(CUR_SRC_DIR "/example.expected.json");
	nlohmann::json dataExpected = nlohmann::json::parse(exampleExpected);

	//remove abs path
	nlohmann::json resJson = res;

	//check
	ASSERT_EQ(dataExpected["getProcMapDistr"], resJson) << " Diff: " << nlohmann::json::diff(dataExpected["getProcMapDistr"], resJson);
}

/**********************************************************/
TEST(TestExtractorHelpers, getFilterdStacks)
{
	//load
	JsonFileIn JsonFileIn(CUR_SRC_DIR "/example.json");
	JsonIn data = JsonFileIn.getRoot();
	MaltProfile profile;
	data.get_to(profile);

	//extract
	Extractor extractor(profile);
	omp_set_num_threads(1);
	FilteredStackList res = extractor.getFilterdStacks([](const InstructionInfosStrRef & location){
		return *location.file == "src/libreader/extractors/tests/example.cpp";
	});

	//load ref
	std::ifstream exampleExpected(CUR_SRC_DIR "/example.expected.json");
	nlohmann::json dataExpected = nlohmann::json::parse(exampleExpected);

	//remove abs path
	nlohmann::json resJson = res;

	//check
	ASSERT_EQ(dataExpected["getFilterdStacks"], resJson) << " Diff: " << nlohmann::json::diff(dataExpected["getProcMapDistr"], resJson);
}

/**********************************************************/
TEST(TestExtractorHelpers, getSummaryV2)
{
	//load
	JsonFileIn JsonFileIn(CUR_SRC_DIR "/example.json");
	JsonIn data = JsonFileIn.getRoot();
	MaltProfile profile;
	data.get_to(profile);

	//extract
	Extractor extractor(profile);
	SummaryV2 res = extractor.getSummaryV2();

	//load ref
	std::ifstream exampleExpected(CUR_SRC_DIR "/example.expected.json");
	nlohmann::json dataExpected = nlohmann::json::parse(exampleExpected);

	//remove abs path
	nlohmann::json resJson = res;

	//check
	ASSERT_EQ(dataExpected["getSummaryV2"], resJson) << " Diff: " << nlohmann::json::diff(dataExpected["getProcMapDistr"], resJson);
}

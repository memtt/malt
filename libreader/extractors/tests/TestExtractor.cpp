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
	EXPECT_EQ(dataExpected["getFlatProfile"], resJson)
		<< "  Diff: "
		<< nlohmann::json::diff(dataExpected["getFlatProfile"], resJson), nlohmann::json::array();
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
TEST(TestExtractorHelpers, getFilterdStacksOnFileLine)
{
	//load
	JsonFileIn JsonFileIn(CUR_SRC_DIR "/example.json");
	JsonIn data = JsonFileIn.getRoot();
	MaltProfile profile;
	data.get_to(profile);

	//extract
	Extractor extractor(profile);
	omp_set_num_threads(1);
	FilteredStackList res = extractor.getFilterdStacksOnFileLine("src/libreader/extractors/tests/example.cpp", 41);

	//load ref
	std::ifstream exampleExpected(CUR_SRC_DIR "/example.expected.json");
	nlohmann::json dataExpected = nlohmann::json::parse(exampleExpected);

	//remove abs path
	nlohmann::json resJson = res;

	//check
	ASSERT_EQ(dataExpected["getFilterdStacksOnFileLine"], resJson) << " Diff: " << nlohmann::json::diff(dataExpected["getFilterdStacksOnFileLine"], resJson);
}

/**********************************************************/
TEST(TestExtractorHelpers, getMaxStackInfoOnFunction)
{
	//load
	JsonFileIn JsonFileIn(CUR_SRC_DIR "/example.json");
	JsonIn data = JsonFileIn.getRoot();
	MaltProfile profile;
	data.get_to(profile);

	//extract
	Extractor extractor(profile);
	omp_set_num_threads(1);
	FlattenMaxStackInfo res = extractor.getMaxStackInfoOnFunction();

	//load ref
	std::ifstream exampleExpected(CUR_SRC_DIR "/example.expected.json");
	nlohmann::json dataExpected = nlohmann::json::parse(exampleExpected);

	//remove abs path
	nlohmann::json resJson = res;

	//check
	ASSERT_EQ(dataExpected["getMaxStackInfoOnFunction"], resJson) << " Diff: " << nlohmann::json::diff(dataExpected["getMaxStackInfoOnFunction"], resJson);
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
	ASSERT_EQ(dataExpected["getSummaryV2"], resJson) << " Diff: " << nlohmann::json::diff(dataExpected["getSummaryV2"], resJson);
}

/**********************************************************/
TEST(TestExtractorHelpers, getSummary)
{
	//load
	JsonFileIn JsonFileIn(CUR_SRC_DIR "/example.json");
	JsonIn data = JsonFileIn.getRoot();
	MaltProfile profile;
	data.get_to(profile);

	//extract
	Extractor extractor(profile);
	Summary res = extractor.getSummary();

	//load ref
	std::ifstream exampleExpected(CUR_SRC_DIR "/example.expected.json");
	nlohmann::json dataExpected = nlohmann::json::parse(exampleExpected);

	//remove abs path
	nlohmann::json resJson = res;

	//check
	ASSERT_EQ(dataExpected["getSummary"], resJson) << " Diff: " << nlohmann::json::diff(dataExpected["getSummary"], resJson);
}

/**********************************************************/
/*TEST(TestExtractorHelpers, getFullTree)
{
	//load
	JsonFileIn JsonFileIn(CUR_SRC_DIR "/example.json");
	JsonIn data = JsonFileIn.getRoot();
	MaltProfile profile;
	data.get_to(profile);

	//extract
	Extractor extractor(profile);
	FullTreeNode res = extractor.getFullTree();

	//load ref
	std::ifstream exampleExpected(CUR_SRC_DIR "/example.expected.json");
	nlohmann::json dataExpected = nlohmann::json::parse(exampleExpected);

	//remove abs path
	nlohmann::json resJson = res;

	//check
	ASSERT_EQ(dataExpected["getFullTree"], resJson) << " Diff: " << nlohmann::json::diff(dataExpected["getFullTree"], resJson);
}*/

/**********************************************************/
TEST(TestExtractorHelpers, getSourceFileMap)
{
	//load
	JsonFileIn JsonFileIn(CUR_SRC_DIR "/example.json");
	JsonIn data = JsonFileIn.getRoot();
	MaltProfile profile;
	data.get_to(profile);

	//extract
	Extractor extractor(profile);
	SourceFileMap res = extractor.getSourceFileMap();

	//check
	ASSERT_TRUE(res.find("src/libreader/extractors/tests/example.cpp") != res.end());
	ASSERT_FALSE(res.find("example.cpp") != res.end());
}


/**********************************************************/
TEST(TestExtractorHelpers, getCallTree)
{
	//load
	JsonFileIn JsonFileIn(CUR_SRC_DIR "/example.json");
	JsonIn data = JsonFileIn.getRoot();
	MaltProfile profile;
	data.get_to(profile);

	//extract
	Extractor extractor(profile);
	nlohmann::json resJson = extractor.getCallTree(-1, 3, 3, 1.0, "", "alloc.count", false);

	//load ref
	std::ifstream exampleExpected(CUR_SRC_DIR "/example.expected.json");
	nlohmann::json dataExpected = nlohmann::json::parse(exampleExpected);

	//check
	ASSERT_EQ(dataExpected["getCallTree"], resJson["dotCode"]) << " Diff: " << nlohmann::json::diff(dataExpected["getCallTree"], resJson["dotCode"]);
	const std::string svg = resJson["svg"];
	ASSERT_TRUE(svg.find("<svg") != -1);
}

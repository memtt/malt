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
bool cstRegenRef = false;

/**********************************************************/
void saveOnRegen(nlohmann::json & ref, const std::string & key, const nlohmann::json & obtained)
{
	if (cstRegenRef) {
		ref[key] = obtained;
		std::ofstream file(CUR_SRC_DIR "/example.expected.json");
		file << ref.dump(1);
	}
}

/**********************************************************/
TEST(TestExtractor, regenData)
{
	if (cstRegenRef) {
		fprintf(stderr, "Regenerate example.json\n");
		setenv("MALT_OPTIONS", "output:indent=true;output:name=" CUR_SRC_DIR "/example.json", 1);
		ASSERT_EQ(0, system(CUR_BUILD_DIR "/extractor-example"));
		system("sed -i " CUR_SRC_DIR "/example.json -e \"s#" BUILD_DIR "/##g\" -e \"s#" SRC_DIR "/##g\"");
	}
}

/**********************************************************/
TEST(TestExtractor, regenDataShouldBeFalse)
{
	ASSERT_FALSE(cstRegenRef);
}

/**********************************************************/
TEST(TestExtractor, constructor)
{
	MaltProfile profile;
	Extractor extractor(profile);
}

/**********************************************************/
TEST(TestExtractor, getFlatProfile)
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
	saveOnRegen(dataExpected, "getFlatProfile", resJson);

	//check
	EXPECT_EQ(dataExpected["getFlatProfile"], resJson)
		<< "  Diff: "
		<< nlohmann::json::diff(dataExpected["getFlatProfile"], resJson), nlohmann::json::array();
}

/**********************************************************/
TEST(TestExtractor, getFlatProfile_full)
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
	saveOnRegen(dataExpected, "getFlatProfile_full", resJson);

	//check
	ASSERT_EQ(nlohmann::json::diff(dataExpected["getFlatProfile_full"], resJson), nlohmann::json::array());
}

/**********************************************************/
TEST(TestExtractor, getProcMapDistr)
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
	saveOnRegen(dataExpected, "getProcMapDistr", resJson);

	//check
	ASSERT_EQ(dataExpected["getProcMapDistr"], resJson) << " Diff: " << nlohmann::json::diff(dataExpected["getProcMapDistr"], resJson);
}

/**********************************************************/
TEST(TestExtractor, getFilterdStacks)
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
		return *location.file == "src/reader/libreader/extractors/tests/example.cpp";
	});

	//load ref
	std::ifstream exampleExpected(CUR_SRC_DIR "/example.expected.json");
	nlohmann::json dataExpected = nlohmann::json::parse(exampleExpected);

	//remove abs path
	nlohmann::json resJson = res;
	saveOnRegen(dataExpected, "getFilterdStacks", resJson);

	//check
	ASSERT_EQ(dataExpected["getFilterdStacks"], resJson) << " Diff: " << nlohmann::json::diff(dataExpected["getProcMapDistr"], resJson);
}

/**********************************************************/
TEST(TestExtractor, getFilterdStacksOnFileLine)
{
	//load
	JsonFileIn JsonFileIn(CUR_SRC_DIR "/example.json");
	JsonIn data = JsonFileIn.getRoot();
	MaltProfile profile;
	data.get_to(profile);

	//extract
	Extractor extractor(profile);
	omp_set_num_threads(1);
	FilteredStackList res = extractor.getFilterdStacksOnFileLine("src/reader/libreader/extractors/tests/example.cpp", 41);

	//load ref
	std::ifstream exampleExpected(CUR_SRC_DIR "/example.expected.json");
	nlohmann::json dataExpected = nlohmann::json::parse(exampleExpected);

	//remove abs path
	nlohmann::json resJson = res;
	saveOnRegen(dataExpected, "getFilterdStacksOnFileLine", resJson);

	//check
	ASSERT_EQ(dataExpected["getFilterdStacksOnFileLine"], resJson) << " Diff: " << nlohmann::json::diff(dataExpected["getFilterdStacksOnFileLine"], resJson);
}

/**********************************************************/
TEST(TestExtractor, getMaxStackInfoOnFunction)
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
	saveOnRegen(dataExpected, "getMaxStackInfoOnFunction", resJson);

	//check
	ASSERT_EQ(dataExpected["getMaxStackInfoOnFunction"], resJson) << " Diff: " << nlohmann::json::diff(dataExpected["getMaxStackInfoOnFunction"], resJson);
}


/**********************************************************/
TEST(TestExtractor, getSummaryV2)
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
	saveOnRegen(dataExpected, "getSummaryV2", resJson);

	//check
	ASSERT_EQ(dataExpected["getSummaryV2"], resJson) << " Diff: " << nlohmann::json::diff(dataExpected["getSummaryV2"], resJson);
}

/**********************************************************/
TEST(TestExtractor, getSummary)
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
	saveOnRegen(dataExpected, "getSummary", resJson);

	//check
	ASSERT_EQ(dataExpected["getSummary"], resJson) << " Diff: " << nlohmann::json::diff(dataExpected["getSummary"], resJson);
}

/**********************************************************/
/*TEST(TestExtractor, getFullTree)
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
	saveOnRegen(dataExpected, "getFullTree", resJson);

	//check
	ASSERT_EQ(dataExpected["getFullTree"], resJson) << " Diff: " << nlohmann::json::diff(dataExpected["getFullTree"], resJson);
}*/

/**********************************************************/
TEST(TestExtractor, getSourceFileMap)
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
	ASSERT_TRUE(res.find("src/reader/libreader/extractors/tests/example.cpp") != res.end());
	ASSERT_FALSE(res.find("example.cpp") != res.end());
}


/**********************************************************/
TEST(TestExtractor, getCallTree)
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
	saveOnRegen(dataExpected, "getCallTree", resJson["dotCode"]);

	//check
	ASSERT_EQ(dataExpected["getCallTree"], resJson["dotCode"]) << " Diff: " << nlohmann::json::diff(dataExpected["getCallTree"], resJson["dotCode"]);
	const std::string svg = resJson["svg"];
	ASSERT_TRUE(svg.find("<svg") != -1);
}

/**********************************************************/
TEST(TestExtractor, getCallStackNextLevel_1)
{
	//load
	JsonFileIn JsonFileIn(CUR_SRC_DIR "/example.json");
	JsonIn data = JsonFileIn.getRoot();
	MaltProfile profile;
	data.get_to(profile);

	//extract
	Extractor extractor(profile);
	LocationFilter filter;
	nlohmann::json resJson = extractor.getCallStackNextLevel(0, 0, filter);

	//load ref
	std::ifstream exampleExpected(CUR_SRC_DIR "/example.expected.json");
	nlohmann::json dataExpected = nlohmann::json::parse(exampleExpected);
	saveOnRegen(dataExpected, "getCallStackNextLevel", resJson);

	//check
	ASSERT_EQ(dataExpected["getCallStackNextLevel"], resJson) << " Diff: " << nlohmann::json::diff(dataExpected["getCallStackNextLevel"], resJson);
}

/**********************************************************/
TEST(TestExtractor, getCallStackNextLevel_2)
{
	//load
	JsonFileIn JsonFileIn(CUR_SRC_DIR "/example.json");
	JsonIn data = JsonFileIn.getRoot();
	MaltProfile profile;
	data.get_to(profile);

	//extract
	Extractor extractor(profile);
	LocationFilter filter;
	CallStackChildList level_0 = extractor.getCallStackNextLevel(1, 1, filter);
	nlohmann::json resJson = extractor.getCallStackNextLevel(level_0.back().parentStackId, level_0.back().parentStackDepth, filter);

	//load ref
	std::ifstream exampleExpected(CUR_SRC_DIR "/example.expected.json");
	nlohmann::json dataExpected = nlohmann::json::parse(exampleExpected);
	saveOnRegen(dataExpected, "getCallStackNextLevel2", resJson);

	//check
	ASSERT_EQ(dataExpected["getCallStackNextLevel2"], resJson) << " Diff: " << nlohmann::json::diff(dataExpected["getCallStackNextLevel2"], resJson);
}

/**********************************************************/
void fillVirtualProfile(MaltProfile & profile)
{
	//fill
	StackInfos stats1;
	stats1.alloc.count = 10;
	StackStat call1{
		{LangAddress{LANG_C, (void*)0x4}, LangAddress{LANG_C, (void*)0x3}, LangAddress{LANG_C, (void*)0x2}, LangAddress{LANG_C, (void*)0x1}},
		(void*)0x1,
		stats1,
	};
	profile.stacks.stats.push_back(call1);

	//fill
	StackInfos stats2;
	stats2.alloc.count = 21;
	StackStat call2{
		{LangAddress{LANG_C, (void*)0x6}, LangAddress{LANG_C, (void*)0x5}, LangAddress{LANG_C, (void*)0x2}, LangAddress{LANG_C, (void*)0x1}},
		(void*)0x2,
		stats2,
	};
	profile.stacks.stats.push_back(call2);

	std::vector<std::string> strings{"0x0", "0x1", "0x2", "0x3", "0x4", "0x5", "0x6"};
	profile.sites.strings = strings;
	for (size_t i = 1 ; i <= 6 ; i++) {
		profile.sites.instr[LangAddress{LANG_C, (void*)i}].file = i;
		profile.sites.instr[LangAddress{LANG_C, (void*)i}].function = i;
		profile.sites.instr[LangAddress{LANG_C, (void*)i}].binary = i;
		profile.sites.instr[LangAddress{LANG_C, (void*)i}].line = i;
	}
}

/**********************************************************/
TEST(TestExtractor, getCallStackNextLevel_virtual_1)
{
	//load
	MaltProfile profile;
	fillVirtualProfile(profile);
	
	//extract
	Extractor extractor(profile);
	LocationFilter filter;
	nlohmann::json resJson = extractor.getCallStackNextLevel(0, 0, filter);
	resJson = ExtractorHelpers::toJsonFiltered(resJson, {"*.alloc.count", "*.hasChild", "*.location.function"});

	//load ref
	std::istringstream file(R"json([{"hasChild":true,"infos":{"alloc":{"count":31}},"location":{"function":"0x2"}}])json");
	nlohmann::json dataExpected = nlohmann::json::parse(file);

	//check
	ASSERT_EQ(dataExpected, resJson) << " Diff: " << nlohmann::json::diff(dataExpected, resJson);
}

/**********************************************************/
TEST(TestExtractor, getCallStackNextLevel_virtual_2)
{
	//load
	MaltProfile profile;
	fillVirtualProfile(profile);
	
	//extract
	Extractor extractor(profile);
	LocationFilter filter;
	nlohmann::json resJson = extractor.getCallStackNextLevel(0, 1, filter);
	resJson = ExtractorHelpers::toJsonFiltered(resJson, {"*.alloc.count", "*.hasChild", "*.location.function"});

	//load ref
	std::istringstream file(R"json([{"hasChild":true,"infos":{"alloc":{"count":10}},"location":{"function":"0x3"}},{"hasChild":true,"infos":{"alloc":{"count":21}},"location":{"function":"0x5"}}])json");
	nlohmann::json dataExpected = nlohmann::json::parse(file);

	//check
	ASSERT_EQ(dataExpected, resJson) << " Diff: " << nlohmann::json::diff(dataExpected, resJson);
}

/**********************************************************/
TEST(TestExtractor, getCallStackNextLevel_virtual_3)
{
	//load
	MaltProfile profile;
	fillVirtualProfile(profile);
	
	//extract
	Extractor extractor(profile);
	LocationFilter filter;
	nlohmann::json resJson = extractor.getCallStackNextLevel(0, 2, filter);
	resJson = ExtractorHelpers::toJsonFiltered(resJson, {"*.alloc.count", "*.hasChild", "*.location.function"});

	//load ref
	std::istringstream file(R"json([{"hasChild":false,"infos":{"alloc":{"count":10}},"location":{"function":"0x4"}}])json");
	nlohmann::json dataExpected = nlohmann::json::parse(file);

	//check
	ASSERT_EQ(dataExpected, resJson) << " Diff: " << nlohmann::json::diff(dataExpected, resJson);
}

/**********************************************************/
TEST(TestExtractor, getCallStackNextLevel_virtual_4)
{
	//load
	MaltProfile profile;
	fillVirtualProfile(profile);
	
	//extract
	Extractor extractor(profile);
	LocationFilter filter;
	nlohmann::json resJson = extractor.getCallStackNextLevel(0, 3, filter);
	resJson = ExtractorHelpers::toJsonFiltered(resJson, {"*.alloc.count", "*.hasChild", "*.location.function"});

	//load ref
	std::istringstream file(R"json([])json");
	nlohmann::json dataExpected = nlohmann::json::parse(file);

	//check
	ASSERT_EQ(dataExpected, resJson) << " Diff: " << nlohmann::json::diff(dataExpected, resJson);
}

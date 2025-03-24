/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/wrappers/tests/TestInstrum.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include <thread>
#include "libinstrum/state/malt.h"
#include "libreader/Profile.hpp"
#include "libreader/extractors/ExtractorHelpers.hpp"
#include <fstream>

/**********************************************************/
namespace MALT
{
	void globalResetForTests();
	void globalDump();
};
using namespace MALTReader;

/**********************************************************/
int maltInitStatus(void)
{
	return 0;
}

/**********************************************************/
void * function_testing_c_malloc()
{
	return malloc(16);
}

/**********************************************************/
TEST(TestInstrum, c_basic_malloc)
{
	//reset MALT & enable
	const std::string profileFile = BINARY_DIR "/malt-c-basic-malloc.json";
	setenv("MALT_OPTIONS", "output:name=" BINARY_DIR "/malt-c-basic-malloc.json", 1);
	MALT::globalResetForTests();

	//make an alloc
	maltEnable();
	void * ptr = function_testing_c_malloc();
	maltDisable();

	//check
	ASSERT_NE(ptr, nullptr);

	//dump & get
	MALT::globalDump();

	//load ref
	nlohmann::json ref = nlohmann::json::parse(std::ifstream(CUR_SRC_DIR "/TestInstrum.json"));

	//conver
	Profile profile(profileFile);
	const MALTReader::Extractor & extractor = profile.getExtractor();
	FlatProfileVector res = extractor.getFlatProfile([](const InstructionInfosStrRef & location, const MALTFormat::StackInfos & infos){
		return *location.function;
	},[](const InstructionInfosStrRef & location, const MALTFormat::StackInfos & infos){
		return location.file->find("/TestInstrum.cpp") != location.file->npos && location.function->find("TestBody") == location.file->npos;
	});
	nlohmann::json resJson = ExtractorHelpers::toJsonFiltered(res, {"*.location", "*.own.alloc", "*.own.free"});
	ExtractorHelpers::jsonRemoveAbsPath(resJson, CUR_SRC_DIR "/");
	resJson = ExtractorHelpers::buildShorterFlatProfileSummary(resJson);

	//check
	ASSERT_EQ(ref["c_basic_malloc"], resJson);
}

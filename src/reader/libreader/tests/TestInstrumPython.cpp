/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 10/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/tests/TestInstrumPython.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include "config.h"
#include <gtest/gtest.h>
#include <thread>
#include "libinstrum/injectors/InjectPythonInit.hpp"
#include "libinstrum/state/malt.h"
#include "libreader/api/Profile.hpp"
#include "libreader/extractors/ExtractorHelpers.hpp"
#include "libinstrum/portability/Python.hpp"
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
TEST(TestInstrumPython, python_basic_array_backtrace)
{
	//reset MALT & enable
	const std::string profileFile = BINARY_DIR "/malt-python-basic-array.json";
	setenv("MALT_OPTIONS", "python:stack=backtrace;python:mix=false;output:name=" BINARY_DIR "/malt-python-basic-array.json", 1);
	MALT::globalResetForTests();

	//make an alloc
	malt_enable();
	const char * argv[] = {"TestInstrum", CUR_SRC_DIR "/main1.py", NULL};
	::Py_Initialize();
	MALT::initPythonInstrumentation(argv[1]);
	int status = ::Py_BytesMain(2, (char**)argv);
	malt_disable();

	//check
	ASSERT_EQ(status, 0);

	//dump & get
	MALT::globalDump();

	//convert
	Profile profile(profileFile);
	const MALTReader::Extractor & extractor = profile.getExtractor();
	FlatProfileVector res = extractor.getFlatProfile([](const InstructionInfosStrRef & location, const MALTFormat::StackInfos & infos){
		return *location.function;
	},[](const InstructionInfosStrRef & location, const MALTFormat::StackInfos & infos){
		return location.file->find("/main1.py") != location.file->npos;
	});
	nlohmann::json resJson = ExtractorHelpers::toJsonFiltered(res, {"*.location", "*.own.alloc.min", "*.own.free.min"});
	ExtractorHelpers::jsonRemoveAbsPath(resJson, CUR_SRC_DIR "/");
	resJson = ExtractorHelpers::buildShorterFlatProfileSummary(resJson, true);

	//load ref
	nlohmann::json ref = nlohmann::json::parse(std::ifstream(CUR_SRC_DIR "/TestInstrumPython.json"));

	//check
	ASSERT_EQ(ref["python_basic_array_backtrace"].dump(1), resJson.dump(1));
}

/**********************************************************/
TEST(TestInstrumPython, python_basic_array_enter_exit)
{
	//reset MALT & enable
	const std::string profileFile = BINARY_DIR "/malt-python-basic-array.json";
	setenv("MALT_OPTIONS", "stack:mode=enter-exit;python:stack=enter-exit;python:mix=false;output:name=" BINARY_DIR "/malt-python-basic-array.json", 1);
	MALT::globalResetForTests();

	//make an alloc
	malt_enable();
	const char * argv[] = {"TestInstrum", CUR_SRC_DIR "/main1.py", NULL};
	::Py_Initialize();
	MALT::initPythonInstrumentation(argv[1]);
	int status = ::Py_BytesMain(2, (char**)argv);
	malt_disable();

	//check
	ASSERT_EQ(status, 0);

	//dump & get
	MALT::globalDump();

	//load ref
	nlohmann::json ref = nlohmann::json::parse(std::ifstream(CUR_SRC_DIR "/TestInstrumPython.json"));

	//convert
	Profile profile(profileFile);
	const MALTReader::Extractor & extractor = profile.getExtractor();
	FlatProfileVector res = extractor.getFlatProfile([](const InstructionInfosStrRef & location, const MALTFormat::StackInfos & infos){
		return *location.function;
	},[](const InstructionInfosStrRef & location, const MALTFormat::StackInfos & infos){
		return location.file->find("/main1.py") != location.file->npos;
	});
	nlohmann::json resJson = ExtractorHelpers::toJsonFiltered(res, {"*.location", "*.own.alloc.min", "*.own.free.min"});
	ExtractorHelpers::jsonRemoveAbsPath(resJson, CUR_SRC_DIR "/");
	resJson = ExtractorHelpers::buildShorterFlatProfileSummary(resJson, true);

	//check
	ASSERT_EQ(ref["python_basic_array_enter_exit"].dump(1), resJson.dump(1));
}

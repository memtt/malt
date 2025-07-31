/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/instrum/core/tests/TestVmaTracker.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

/**********************************************************/
#include <cstdio>
#include <cstdlib>
#include <string>
#include <gtest/gtest.h>
#include "reader/Profile.hpp"
#include "reader/extractors/ExtractorHelpers.hpp"
#include <fstream>

/**********************************************************/
using namespace MALTReader;

/**********************************************************/
TEST(TestFullAnalysis, basic_run)
{
	// set env
	const int status_env = setenv("MALT_OPTIONS", "output:indent=true;output:name=" BINARY_DIR "/malt-current-out-basic-run.json", 1);
	ASSERT_EQ(status_env, 0);

	//run & check
	const int status_sys = system(EXE_PATH "/simple-case-finstr-linked > /dev/null");
	ASSERT_EQ(status_sys, 0);

	//check file has been generated
	FILE * fp = fopen(BINARY_DIR "/malt-current-out-basic-run.json", "r");
	fclose(fp);
	ASSERT_NE(fp, nullptr);
}

/**********************************************************/
TEST(TestFullAnalysis, basic_run_and_check_file_line_counter)
{
	// produce profile
	const std::string profileFile = BINARY_DIR "/malt-current-out-basic-run-and-check.json";
	const int status_env = setenv("MALT_OPTIONS", "output:indent=true;output:name=" BINARY_DIR "/malt-current-out-basic-run-and-check.json", 1);
	ASSERT_EQ(status_env, 0);
	const int status_sys = system(EXE_PATH "/simple-case-finstr-linked > /dev/null");
	ASSERT_EQ(status_sys, 0);

	//load ref
	nlohmann::json ref = nlohmann::json::parse(std::ifstream(CUR_SRC_DIR "/TestFullAnalysis.json"));

	//convert
	Profile profile(profileFile);
	const MALTReader::Extractor & extractor = profile.getExtractor();
	FlatProfileVector res = extractor.getFlatProfile([](const InstructionInfosStrRef & location, const MALTFormat::StackInfos & infos){
		char buffer[4096] = "";
		snprintf(buffer, sizeof(buffer), "%s:%s:%zd", location.file->c_str(), location.function->c_str(), location.line);
		return std::string(buffer);
	},[](const InstructionInfosStrRef & location, const MALTFormat::StackInfos & infos){
		return location.file->find("/simple-case.cpp") != location.file->npos 
			&& location.function->find("gblArray") == location.function->npos 
			&& location.function->find("gblArray") == location.function->npos
			&& location.function->find("__static_initialization_and_destruction_0") ==  location.function->npos
			&& location.line != 222 && location.line != 216;
	});
	nlohmann::json resJson = ExtractorHelpers::toJsonFiltered(res, {"*.location", "*.own.alloc", "*.own.free"});
	ExtractorHelpers::jsonRemoveAbsPath(resJson, SRC_DIR "/src/lib/instrum/tests/");
	resJson = ExtractorHelpers::buildShorterFlatProfileSummary(resJson);

	//check
	ASSERT_EQ(ref["basic_run_and_check_file_line_counter"], resJson)
		<< "  Diff: "
		<< nlohmann::json::diff(ref["basic_run_and_check_file_line_counter"], resJson);
}

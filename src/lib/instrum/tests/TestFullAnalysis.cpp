/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/insturm/core/tests/TestVmaTracker.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

/**********************************************************/
#include <cstdio>
#include <cstdlib>
#include <gtest/gtest.h>
#include <common/Helpers.hpp>

/**********************************************************/
TEST(TestFullAnalysis, basic_run)
{
	// set env
	const int status_env = setenv("MALT_OPTIONS", "output:indent=true;output:name=" EXE_PATH "/malt-current-out-basic-run.json", 1);
	ASSERT_EQ(status_env, 0);

	//run & check
	const int status_sys = system(EXE_PATH "/simple-case-finstr-linked > /dev/null");
	ASSERT_EQ(status_sys, 0);

	//check file has been generated
	FILE * fp = fopen(EXE_PATH "/malt-current-out-basic-run.json", "r");
	fclose(fp);
	ASSERT_NE(fp, nullptr);
}

TEST(TestFullAnalysis, basic_run_and_check_file_line_counter)
{
	// produce profile
	const int status_env = setenv("MALT_OPTIONS", "output:indent=true;output:name=" EXE_PATH "/malt-current-out-basic-run-and-check.json", 1);
	ASSERT_EQ(status_env, 0);
	const int status_sys = system(EXE_PATH "/simple-case-finstr-linked > /dev/null");
	ASSERT_EQ(status_sys, 0);

	//extract infis
	std::string result = MALT::Helpers::simpleProfileDump(EXE_PATH "/malt-current-out-basic-run-and-check.json", SRC_PATH "/src/lib/insturm/tests/simple-case.cpp");
		 
	//load file
	std::string ref = MALT::Helpers::loadFullFile(SRC_PATH "/src/lib/insturm/tests/malt-current-out-basic-run-and-check-extract-ref.txt");

	//MALT::Helpers::writeFullFile(SRC_PATH "/src/lib/insturm/tests/malt-current-out-basic-run-and-check-extract-ref.txt", result);

	//compare
	ASSERT_EQ(result, ref);
}
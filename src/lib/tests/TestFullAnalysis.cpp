/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/core/tests/TestVmaTracker.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

/**********************************************************/
#include <cstdio>
#include <cstdlib>
#include <gtest/gtest.h>

/**********************************************************/
static std::string loadFullFile(std::string fname)
{
	//output
	std::string result;

	//open
	FILE * fp = fopen(fname.c_str(), "r");
	if (fp == NULL)
		return result;

	//load
	while (!feof(fp)) {
		char buffer[4096];
		ssize_t size = fread(buffer, 1,sizeof(buffer)-1, fp);
		buffer[size] = '\0';
		result += buffer;
	}

	//close
	fclose(fp);

	//ok
	return result;
}

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
	const int status_end_2 = setenv("NODE_PATH", BUILD_PATH "/src/webview/node_modules/", 1);
	const int status_sys_2 = system("node "
		 SRC_PATH "/src/webview/malt-simple-dump.js "
		 "-i " EXE_PATH "/malt-current-out-basic-run-and-check.json "
		 "-s " SRC_PATH "/src/lib/tests/simple-case.cpp "
		 "| sed -e 's#" SRC_PATH "##g' "
		 "> " EXE_PATH "/malt-current-out-basic-run-and-check-extract.txt"
	);

	//load file
	std::string result = loadFullFile(EXE_PATH "/malt-current-out-basic-run-and-check-extract.txt");
	std::string ref = loadFullFile(SRC_PATH "/src/lib/tests/malt-current-out-basic-run-and-check-extract-ref.txt");

	//compare
	ASSERT_EQ(result, ref);
}
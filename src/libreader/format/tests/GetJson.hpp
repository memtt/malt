/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/core/tests/TestAllocTraceFile.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2022 - 2024
***********************************************************/

#ifndef MALT_FORMAT_TEST_GET_JSON_HPP
#define MALT_FORMAT_TEST_GET_JSON_HPP

/**********************************************************/
#include <gtest/gtest.h>
#include <fstream>
#include "../MaltProfile.hpp"

/**********************************************************/
static void get_json(nlohmann::json & data)
{
	//calc profile fname
	const std::string fname = CUR_BUILD_DIR "/malt-simple-case-finstr-linked.json";

	//check if exist
	FILE * fp = fopen(fname.c_str(), "r");
	if (fp == nullptr) {
		//generate
		int status = system("MALT_OPTIONS='output:indent=true;output:name=" CUR_BUILD_DIR "/malt-%1.%3' " BUILD_DIR "/src/lib/tests/simple-case-finstr-linked");
	} else {
		fclose(fp);
	}

	//load
	std::ifstream f(fname);
	data = nlohmann::json::parse(f);
}

#endif //MALT_FORMAT_TEST_GET_JSON_HPP

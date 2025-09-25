/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/format/tests/GetJson.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#ifndef MALT_FORMAT_TEST_GET_JSON_HPP
#define MALT_FORMAT_TEST_GET_JSON_HPP

/**********************************************************/
#include <gtest/gtest.h>
#include <fstream>
#include "../MaltProfile.hpp"
#include "../Json.hpp"

/**********************************************************/
static void get_json(nlohmann::json & data)
{
	//calc profile fname
	const std::string fname = CUR_BUILD_DIR "/malt-simple-case-finstr-linked.json";

	std::ifstream f(fname);
	data = nlohmann::json::parse(f);
}

/**********************************************************/
static void get_json_in(MALTFormat::JsonIn & data)
{
	//calc profile fname
	const std::string fname = CUR_BUILD_DIR "/malt-simple-case-finstr-linked.json";

	//check if exist
	FILE * fp = fopen(fname.c_str(), "r");
	static bool doneOnce = false;
	if (fp == nullptr || doneOnce == false) {
		//generate
		int status = system("MALT_OPTIONS='output:indent=true;output:name=" CUR_BUILD_DIR "/malt-%1.%3' " BUILD_DIR "/src/libinstrum/tests/simple-case-finstr-linked");
		doneOnce = true;
	} else {
		fclose(fp);
	}

	//load
	#ifdef USE_INTERNAL_JSON_IN
		static MALTJson::JsonFile file(fname);
		data = file.getRoot();
	#else
		std::ifstream f(fname);
		data = nlohmann::json::parse(f);
	#endif
}

#endif //MALT_FORMAT_TEST_GET_JSON_HPP

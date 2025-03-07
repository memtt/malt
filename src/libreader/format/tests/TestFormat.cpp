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
#include "../MaltProfile.hpp"

/**********************************************************/
using namespace MALTFormat;

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

/**********************************************************/
/*TEST(TestFormat, load_save_load)
{
	//get json
	nlohmann::json data;
	get_json(data);

	//convert in / back
	MaltProfile profile = data;
	const nlohmann::json asJson = profile;

	//check
	ASSERT_EQ(data, asJson);
}*/

/**********************************************************/
TEST(TestFormat, field_config)
{
	//get json
	nlohmann::json data;
	get_json(data);

	//convert in / back
	MALTFormat::Config profile = data["config"];
	const nlohmann::json asJson = profile;

	//check
	const nlohmann::json empty = std::vector<int>();
	ASSERT_EQ(nlohmann::json::diff(data["config"], asJson), empty);
}

/**********************************************************/
TEST(TestFormat, field_run)
{
	//get json
	nlohmann::json data;
	get_json(data);

	//convert in / back
	MALTFormat::Run profile = data["run"];
	const nlohmann::json asJson = profile;

	//check
	const nlohmann::json empty = std::vector<int>();
	ASSERT_EQ(nlohmann::json::diff(data["run"], asJson), empty);
}

/**********************************************************/
TEST(TestFormat, field_globals)
{
	//get json
	nlohmann::json data;
	get_json(data);

	//convert in / back
	MALTFormat::Globals profile = data["globals"];
	const nlohmann::json asJson = profile;

	//check
	const nlohmann::json empty = std::vector<int>();
	ASSERT_EQ(nlohmann::json::diff(data["globals"], asJson), empty);
}

/**********************************************************/
TEST(TestFormat, field_leaks)
{
	//get json
	nlohmann::json data;
	get_json(data);

	//convert in / back
	MALTFormat::Leaks profile = data["leaks"];
	const nlohmann::json asJson = profile;

	//check
	const nlohmann::json empty = std::vector<int>();
	ASSERT_EQ(nlohmann::json::diff(data["leaks"], asJson), empty);
}

/**********************************************************/
TEST(TestFormat, field_mem_stats)
{
	//get json
	nlohmann::json data;
	get_json(data);

	//convert in / back
	MALTFormat::MemStats profile = data["memStats"];
	const nlohmann::json asJson = profile;

	//check
	const nlohmann::json empty = std::vector<int>();
	//ASSERT_EQ(data["memStats"], asJson);
	ASSERT_EQ(nlohmann::json::diff(data["memStats"], asJson), empty);
}

/**********************************************************/
TEST(TestFormat, field_scatter)
{
	//get json
	nlohmann::json data;
	get_json(data);

	//convert in / back
	MALTFormat::Scatter profile = data["scatter"];
	const nlohmann::json asJson = profile;

	//check
	const nlohmann::json empty = std::vector<int>();
	ASSERT_EQ(data["scatter"], asJson);
}

/**********************************************************/
TEST(TestFormat, field_sites)
{
	//get json
	nlohmann::json data;
	get_json(data);

	//convert in / back
	MALTFormat::Sites profile = data["sites"];
	const nlohmann::json asJson = profile;

	//check
	const nlohmann::json empty = std::vector<int>();
	EXPECT_EQ(data["sites"], asJson);
	EXPECT_EQ(nlohmann::json::diff(data["sites"], asJson), empty);
}

/**********************************************************/
TEST(TestFormat, field_threads)
{
	//get json
	nlohmann::json data;
	get_json(data);

	//convert in / back
	MALTFormat::Threads profile = data["threads"];
	const nlohmann::json asJson = profile;

	//check
	const nlohmann::json empty = std::vector<int>();
	ASSERT_EQ(nlohmann::json::diff(data["threads"], asJson), empty);
}

/**********************************************************/
TEST(TestFormat, field_timeline)
{
	//get json
	nlohmann::json data;
	get_json(data);

	//convert in / back
	MALTFormat::Timeline profile = data["timeline"];
	const nlohmann::json asJson = profile;

	//check
	const nlohmann::json empty = std::vector<int>();
	ASSERT_EQ(nlohmann::json::diff(data["timeline"], asJson), empty);
}

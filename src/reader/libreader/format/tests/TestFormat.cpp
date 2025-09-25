/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/format/tests/TestFormat.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include <fstream>
#include "../MaltProfile.hpp"
#include "GetJson.hpp"

/**********************************************************/
using namespace MALTFormat;

/**********************************************************/
TEST(TestFormat, load_save_load)
{
	//get json in
	JsonIn dataIn(nullptr);
	get_json_in(dataIn);

	//get json
	nlohmann::json data;
	get_json(data);

	//convert in / back
	MaltProfile profile;
	dataIn.get_to(profile);
	const nlohmann::json asJson = profile;

	//check
	const nlohmann::json empty = std::vector<int>();
	ASSERT_EQ(nlohmann::json::diff(data, asJson), empty);
}

/**********************************************************/
TEST(TestFormat, field_config)
{
	//get json in
	JsonIn dataIn(nullptr);
	get_json_in(dataIn);

	
	//get json
	nlohmann::json data;
	get_json(data);

	//convert in / back
	MALTFormat::Config profile;
	dataIn["config"].get_to(profile);
	const nlohmann::json asJson = profile;

	//check
	const nlohmann::json empty = std::vector<int>();
	ASSERT_EQ(nlohmann::json::diff(data["config"], asJson), empty);
}

/**********************************************************/
TEST(TestFormat, field_run)
{
	//get json in
	JsonIn dataIn(nullptr);
	get_json_in(dataIn);

	
	//get json
	nlohmann::json data;
	get_json(data);

	//convert in / back
	MALTFormat::Run profile;
	dataIn["run"].get_to(profile);
	const nlohmann::json asJson = profile;

	//check
	const nlohmann::json empty = std::vector<int>();
	ASSERT_EQ(nlohmann::json::diff(data["run"], asJson), empty);
}

/**********************************************************/
TEST(TestFormat, field_globals)
{
	//get json in
	JsonIn dataIn(nullptr);
	get_json_in(dataIn);

	
	//get json
	nlohmann::json data;
	get_json(data);

	//convert in / back
	MALTFormat::Globals profile;
	dataIn["globals"].get_to(profile);
	const nlohmann::json asJson = profile;

	//check
	const nlohmann::json empty = std::vector<int>();
	ASSERT_EQ(nlohmann::json::diff(data["globals"], asJson), empty);
}

/**********************************************************/
TEST(TestFormat, field_leaks)
{
	//get json in
	JsonIn dataIn(nullptr);
	get_json_in(dataIn);

	
	//get json
	nlohmann::json data;
	get_json(data);

	//convert in / back
	MALTFormat::Leaks profile;
	dataIn["leaks"].get_to(profile);
	const nlohmann::json asJson = profile;

	//check
	const nlohmann::json empty = std::vector<int>();
	ASSERT_EQ(nlohmann::json::diff(data["leaks"], asJson), empty);
}

/**********************************************************/
TEST(TestFormat, field_mem_stats)
{
	//get json in
	JsonIn dataIn(nullptr);
	get_json_in(dataIn);
	
	//get json
	nlohmann::json data;
	get_json(data);

	//convert in / back
	MALTFormat::MemStats profile;
	dataIn["memStats"].get_to(profile);
	const nlohmann::json asJson = profile;

	//check
	const nlohmann::json empty = std::vector<int>();
	//ASSERT_EQ(data["memStats"], asJson);
	ASSERT_EQ(nlohmann::json::diff(data["memStats"], asJson), empty);
}

/**********************************************************/
TEST(TestFormat, field_scatter)
{
	//get json in
	JsonIn dataIn(nullptr);
	get_json_in(dataIn);

	//get json
	nlohmann::json data;
	get_json(data);

	//convert in / back
	MALTFormat::Scatter profile;
	dataIn["scatter"].get_to(profile);
	const nlohmann::json asJson = profile;

	//check
	const nlohmann::json empty = std::vector<int>();
	ASSERT_EQ(data["scatter"], asJson);
}

/**********************************************************/
TEST(TestFormat, field_sites)
{
	//get json in
	JsonIn dataIn(nullptr);
	get_json_in(dataIn);

	//get json
	nlohmann::json data;
	get_json(data);

	//convert in / back
	MALTFormat::Sites profile;
	dataIn["sites"].get_to(profile);
	const nlohmann::json asJson = profile;

	//check
	const nlohmann::json empty = std::vector<int>();
	EXPECT_EQ(data["sites"], asJson);
	EXPECT_EQ(nlohmann::json::diff(data["sites"], asJson), empty);
}

/**********************************************************/
TEST(TestFormat, field_threads)
{
	//get json in
	JsonIn dataIn(nullptr);
	get_json_in(dataIn);

	//get json
	nlohmann::json data;
	get_json(data);

	//convert in / back
	MALTFormat::Threads profile;
	dataIn["threads"].get_to(profile);
	const nlohmann::json asJson = profile;

	//check
	const nlohmann::json empty = std::vector<int>();
	ASSERT_EQ(nlohmann::json::diff(data["threads"], asJson), empty);
}

/**********************************************************/
TEST(TestFormat, field_timeline)
{
	//get json in
	JsonIn dataIn(nullptr);
	get_json_in(dataIn);

	//get json
	nlohmann::json data;
	get_json(data);

	//convert in / back
	MALTFormat::Timeline profile;
	dataIn["timeline"].get_to(profile);
	const nlohmann::json asJson = profile;

	//check
	const nlohmann::json empty = std::vector<int>();
	ASSERT_EQ(nlohmann::json::diff(data["timeline"], asJson), empty);
}

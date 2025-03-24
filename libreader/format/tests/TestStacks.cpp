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
#include "GetJson.hpp"

/**********************************************************/
using namespace MALTFormat;

/**********************************************************/
TEST(TestFormatStacks, load_save_load)
{
	//get json in
	JsonIn dataIn(nullptr);
	get_json_in(dataIn);

	//get json
	nlohmann::json data;
	get_json(data);

	//convert in / back
	MALTFormat::Stacks profile;
	#pragma omp parallel
	{
		#pragma omp single
		dataIn["stacks"].get_to(profile);
	}
	const nlohmann::json asJson = profile;

	//check
	const nlohmann::json empty = std::vector<int>();
	ASSERT_EQ(nlohmann::json::diff(data["stacks"], asJson), empty);
}

/**********************************************************/
TEST(TestFormatCountMinMaxSum, merge)
{
	CountMinMaxSum a{1000, 256, 512, 256000};
	CountMinMaxSum b{2000, 128, 1024, 256000};
	a.merge(b);
	EXPECT_EQ(a.count, 3000);
	EXPECT_EQ(a.min, 128);
	EXPECT_EQ(a.max, 1024);
	EXPECT_EQ(a.sum, 512000);
}

/**********************************************************/
TEST(TestFormatStackInfos, merge)
{
	MALTFormat::StackInfos a{25, 10, 1, {1000, 256, 512, 256000}, {1001, 257, 513, 256001}, {1002, 258, 514, 256002}, 2048, 50, 4096};
	MALTFormat::StackInfos b{250, 100, 10, {10000, 2560, 5120, 2560000}, {10010, 2570, 5130, 2560010}, {10020, 2580, 5140, 2560020}, 20480, 500, 40960};
	MALTFormat::StackInfos exected{275, 110, 11, {11000, 256, 5120, 256000+2560000}, {11011, 257, 5130, 256001+2560010}, {11022, 258, 5140, 256002+2560020}, 2048+20480, 550, 40960+4096};

	a.merge(b);
	EXPECT_EQ(nlohmann::json(a), nlohmann::json(exected));
}

/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/common/tests/TestHelpers.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include "../Helpers.hpp"

/**********************************************************/
using namespace MALT;

/**********************************************************/
const char cstData1[] = "\
This is a file to load.\n\
With several lines.\n\
For testing Helpers::loadFullFile.\n\
";

/**********************************************************/
TEST(TestHelpers, printValue)
{
	//basic
	std::stringstream buffer1;
	Helpers::printValue(buffer1, 256, "B");
	ASSERT_EQ(buffer1.str(), "256.0  B");

	//Kilo
	std::stringstream buffer2;
	Helpers::printValue(buffer2, 1024, "B");
	ASSERT_EQ(buffer2.str(), "  1.0 KB");

	//Kilo
	std::stringstream buffer4;
	Helpers::printValue(buffer4, 1024+512, "B");
	ASSERT_EQ(buffer4.str(), "  1.5 KB");

	//Mega
	std::stringstream buffer3;
	Helpers::printValue(buffer3, 1024*1024, "B");
	ASSERT_EQ(buffer3.str(), "  1.0 MB");
}

/**********************************************************/
TEST(TestHelpers, loadFullFile)
{
	const std::string value = Helpers::loadFullFile(TEST_SRC_DIR "/data1.txt");
	ASSERT_EQ(value, cstData1);
}

/**********************************************************/
TEST(TestHelpers, writeFullFile)
{
	bool status = Helpers::writeFullFile(TEST_BUILD_DIR "/data-full-write.txt", cstData1);
	ASSERT_TRUE(status);
	const std::string value = Helpers::loadFullFile(TEST_BUILD_DIR "/data-full-write.txt");
	ASSERT_EQ(value, cstData1);
}

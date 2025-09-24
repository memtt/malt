/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
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

/**********************************************************/
TEST(TestHelpers, rankStrToIntSet_empty)
{
	IntSet ranks = Helpers::rankStrToIntSet("");
	ASSERT_TRUE(ranks.empty());
}

/**********************************************************/
TEST(TestHelpers, rankStrToIntSet_single)
{
	IntSet ranks = Helpers::rankStrToIntSet("1");
	ASSERT_TRUE(ranks.find(1) != ranks.end());
	ASSERT_EQ(ranks.size(), 1);
}

/**********************************************************/
TEST(TestHelpers, rankStrToIntSet_many)
{
	IntSet ranks = Helpers::rankStrToIntSet("1,2,3,4");
	ASSERT_TRUE(ranks.find(1) != ranks.end());
	ASSERT_TRUE(ranks.find(2) != ranks.end());
	ASSERT_TRUE(ranks.find(3) != ranks.end());
	ASSERT_TRUE(ranks.find(4) != ranks.end());
	ASSERT_EQ(ranks.size(), 4);
}

/**********************************************************/
TEST(TestHelpers, rankStrToIntSet_single_range)
{
	IntSet ranks = Helpers::rankStrToIntSet("1-4");
	ASSERT_TRUE(ranks.find(1) != ranks.end());
	ASSERT_TRUE(ranks.find(2) != ranks.end());
	ASSERT_TRUE(ranks.find(3) != ranks.end());
	ASSERT_TRUE(ranks.find(4) != ranks.end());
	ASSERT_EQ(ranks.size(), 4);
}

/**********************************************************/
TEST(TestHelpers, rankStrToIntSet_list_and_range)
{
	IntSet ranks = Helpers::rankStrToIntSet("1,2-3,4");
	ASSERT_TRUE(ranks.find(1) != ranks.end());
	ASSERT_TRUE(ranks.find(2) != ranks.end());
	ASSERT_TRUE(ranks.find(3) != ranks.end());
	ASSERT_TRUE(ranks.find(4) != ranks.end());
	ASSERT_EQ(ranks.size(), 4);
}

/**********************************************************/
int main(int argc, char ** argv)
{
	//init internal allocator
	gblInternaAlloc = new SimpleAllocator(true);
	
	// This allows the user to override the flag on the command line.
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

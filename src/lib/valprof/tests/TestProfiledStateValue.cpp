/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <sstream>
#include <gtest/gtest.h>
#include <valprof/ProfiledStateValue.hpp>

/***************** USING NAMESPACE ******************/
using namespace MATT;

/********************** CONSTS **********************/
const char CST_STRING_1[] = "{\n\t\"min\":[10, 30, 40, 50, 60],\n\t\"max\":[20, 30, 40, 50, 60],\n\t\"timestamp\":[1, 2, 3, 4, 5],\n\t\"peakMemory\":70,\n\t\"peakTimesteamp\":6,\n\t\"linearIndex\":true\n}";
const char CST_STRING_2[] = "{\n\t\"min\":[10, 40, 190, 220, 290, 350, 480, 680, 730, 880],\n\t\"max\":[30, 100, 210, 280, 340, 410, 560, 720, 870, 940],\n\t\"timestamp\":[2, 9, 20, 27, 33, 40, 55, 71, 86, 93],\n\t\"peakMemory\":1000,\n\t\"peakTimesteamp\":99,\n\t\"linearIndex\":true\n}";

/*******************  FUNCTION  *********************/
TEST(ProfiledValue,constructor)
{
	ProfiledStateValue profile(10);
}

/*******************  FUNCTION  *********************/
TEST(ProfiledValue,test_1)
{
	ProfiledStateValue profile(10,true);
	
	for (int i = 0 ; i < 7 ; i++)
		profile.onDeltaEvent(10);
	
	std::stringstream buffer;
	htopml::convertToJson(buffer,profile);
	
	EXPECT_EQ(CST_STRING_1,buffer.str());
}

/*******************  FUNCTION  *********************/
TEST(ProfiledValue,test_2)
{
	ProfiledStateValue profile(10,true);
	
	for (int i = 0 ; i < 100 ; i++)
		profile.onDeltaEvent(10);
	
	std::stringstream buffer;
	htopml::convertToJson(buffer,profile);
	
	EXPECT_EQ(CST_STRING_2,buffer.str());
}
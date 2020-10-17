/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <sstream>
#include <gtest/gtest.h>
#include <valprof/ProfiledStateValue.hpp>

/***************** USING NAMESPACE ******************/
using namespace MALT;

/********************** CONSTS **********************/
const char CST_STRING_1[] = "{\n\t\"min\":[10, 30, 40, 50, 60],\n\t\"max\":[20, 30, 40, 50, 60],\n\t\"index\":[1, 2, 3, 4, 5],\n\t\"peakMemory\":70,\n\t\"peakIndex\":6,\n\t\"linearIndex\":true\n}";
const char CST_STRING_2[] = "{\n\t\"min\":[10, 130, 260, 280, 590, 640],\n\t\"max\":[120, 210, 270, 480, 630, 880],\n\t\"index\":[11, 20, 26, 47, 62, 87],\n\t\"peakMemory\":1000,\n\t\"peakIndex\":99,\n\t\"linearIndex\":true\n}";


/*******************  FUNCTION  *********************/
TEST(ProfiledValue,constructor)
{
	ProfiledStateValue profile(10);
}

/*******************  FUNCTION  *********************/
TEST(ProfiledValue,test_1)
{
	ProfiledStateValue profile(10,true);
	profile.disableTimestamp();
	
	for (int i = 0 ; i < 7 ; i++)
		profile.onDeltaEvent(10,NULL);
	
	std::stringstream buffer;
	htopml::convertToJson(buffer,profile);
	
	EXPECT_EQ(CST_STRING_1,buffer.str());
}

/*******************  FUNCTION  *********************/
TEST(ProfiledValue,test_2)
{
	ProfiledStateValue profile(10,true);
	profile.disableTimestamp();
	
	for (int i = 0 ; i < 100 ; i++)
		profile.onDeltaEvent(10,NULL);
	
	std::stringstream buffer;
	htopml::convertToJson(buffer,profile);
	
	EXPECT_EQ(CST_STRING_2,buffer.str());
}
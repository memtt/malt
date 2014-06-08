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
#include <valprof/ProfiledCumulValue.hpp>

/***************** USING NAMESPACE ******************/
using namespace MATT;

/********************** CONSTS **********************/
const char CST_STRING_1[] = "{\n\t\"startTime\":0,\n\t\"steps\":10,\n\t\"values\":[10, 10, 10, 10, 10, 10, 10, 0, 0, 0]\n}";
const char CST_STRING_2[] = "{\n\t\"startTime\":0,\n\t\"steps\":10,\n\t\"values\":[16, 16, 16, 16, 16, 16, 4, 0, 0, 0]\n}";

/*******************  FUNCTION  *********************/
TEST(ProfiledCumulValue,constructor)
{
	ProfiledCumulValue profile(10);
}

/*******************  FUNCTION  *********************/
TEST(ProfiledCumulValue,test_1)
{
	ProfiledCumulValue profile(10,true);
	
	for (int i = 0 ; i < 7 ; i++)
		profile.push(10);
	
	std::stringstream buffer;
	htopml::convertToJson(buffer,profile);
	
	EXPECT_EQ(CST_STRING_1,buffer.str());
}

/*******************  FUNCTION  *********************/
TEST(ProfiledCumulValue,test_2)
{
	ProfiledCumulValue profile(10,true);
	
	for (int i = 0 ; i < 100 ; i++)
		profile.push(1);
	
	std::stringstream buffer;
	htopml::convertToJson(buffer,profile);
	
	EXPECT_EQ(CST_STRING_2,buffer.str());
}
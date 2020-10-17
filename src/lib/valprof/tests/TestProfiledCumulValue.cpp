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
#include <valprof/ProfiledCumulValue.hpp>

/***************** USING NAMESPACE ******************/
using namespace MALT;

/********************** CONSTS **********************/
const char CST_STRING_1[] = "{\n\t\"startTime\":0,\n\t\"scale\":1,\n\t\"endTime\":6,\n\t\"values\":[10, 10, 10, 10, 10, 10, 10]\n}";
const char CST_STRING_2[] = "{\n\t\"startTime\":0,\n\t\"scale\":16,\n\t\"endTime\":99,\n\t\"values\":[16, 16, 16, 16, 16, 16, 4]\n}";

/*******************  FUNCTION  *********************/
TEST(ProfiledCumulValue,constructor)
{
	ProfiledCumulValue<size_t> profile(10,false,true);
}

/*******************  FUNCTION  *********************/
TEST(ProfiledCumulValue,test_1)
{
	ProfiledCumulValue<size_t> profile(10,true,true);
	
	for (int i = 0 ; i < 7 ; i++)
		profile.push(10);
	
	std::stringstream buffer;
	htopml::convertToJson(buffer,profile);
	
	EXPECT_EQ(CST_STRING_1,buffer.str());
}

/*******************  FUNCTION  *********************/
TEST(ProfiledCumulValue,test_2)
{
	ProfiledCumulValue<size_t> profile(10,true,true);
	
	for (int i = 0 ; i < 100 ; i++)
		profile.push(1);
	
	std::stringstream buffer;
	htopml::convertToJson(buffer,profile);
	
	EXPECT_EQ(CST_STRING_2,buffer.str());
}
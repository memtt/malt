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
#include <json/ConvertToJson.h>
#include <valprof/ProfiledValue.hpp>

/***************** USING NAMESPACE ******************/
using namespace MALT;

/********************** CONSTS **********************/
static void * CST_FUNC_1 = (void*)0xA;
static void * CST_FUNC_2 = (void*)0xB;
static const char * CST_STRING_1 = "{\n\t\"start\":0,\n\t\"fields\":[\"max\"],\n\t\"perPoints\":1,\n\t\"peak\":3,\n\t\"values\":[1, 1, 2, 2, 3],\n\t\"callsite\":[\"0xa\", \"0xa\", \"0xa\", \"0xa\", \"0xa\"]\n}";
static const char * CST_STRING_2 = "{\n\t\"start\":0,\n\t\"fields\":[\"max\"],\n\t\"perPoints\":2,\n\t\"peak\":9,\n\t\"values\":[0, 1, 2, 3, 4, 5, 6, 7, 8, 9],\n\t\"callsite\":[\"0xa\", \"0xa\", \"0xa\", \"0xa\", \"0xa\", \"0xa\", \"0xa\", \"0xa\", \"0xa\", \"0xa\"]\n}";
static const char * CST_STRING_3 = "{\n\t\"start\":0,\n\t\"fields\":[\"max\"],\n\t\"perPoints\":4,\n\t\"peak\":19,\n\t\"values\":[1, 3, 5, 7, 9, 11, 13, 15, 17, 19],\n\t\"callsite\":[\"0xa\", \"0xa\", \"0xa\", \"0xa\", \"0xa\", \"0xa\", \"0xa\", \"0xa\", \"0xa\", \"0xa\"]\n}";
static const char * CST_STRING_4 = "{\n\t\"start\":0,\n\t\"fields\":[\"max\"],\n\t\"perPoints\":4,\n\t\"peak\":39,\n\t\"values\":[3, 7, 11, 15, 19, 23, 27, 31, 35, 39],\n\t\"callsite\":[\"0xb\", \"0xb\", \"0xb\", \"0xb\", \"0xb\", \"0xb\", \"0xb\", \"0xb\", \"0xb\", \"0xb\"]\n}";
static const char * CST_STRING_5 = "{\n\t\"start\":0,\n\t\"fields\":[\"max\"],\n\t\"perPoints\":1,\n\t\"peak\":4,\n\t\"values\":[1, 1, 1, 1, 3, 3, 4],\n\t\"callsite\":[\"0xa\", \"0xa\", \"(nil)\", \"(nil)\", \"0xa\", \"0xa\", \"0xa\"]\n}";


/*******************  FUNCTION  *********************/
TEST(ProfiledValue,constructor)
{
	ProfiledStateMaxInt profile(10);
}

/*******************  FUNCTION  *********************/
TEST(ProfiledValue,push_only_5)
{
	//setup
	ProfiledStateMaxInt profile(10);
	
	//fill
	ticks t = 0;
	profile.push(t++,1,CST_FUNC_1);
	profile.push(t++,1,CST_FUNC_1);
	profile.push(t++,2,CST_FUNC_1);
	profile.push(t++,2,CST_FUNC_1);
	profile.push(t++,3,CST_FUNC_1);
	
	//output
	std::stringstream buffer;
	profile.flush();
	htopml::convertToJson(buffer,profile);
	
	EXPECT_EQ(CST_STRING_1,buffer.str());
}

/*******************  FUNCTION  *********************/
TEST(ProfiledValue,push_max)
{
	//setup
	ProfiledStateMaxInt profile(10);
	
	//fill
	ticks t = 0;
	for (int i = 0 ; i < 10 ; i++)
	{
		profile.push(t++,i,CST_FUNC_1);
		profile.push(t++,i,CST_FUNC_2);
	}
	
	//output
	std::stringstream buffer;
	profile.flush();
	htopml::convertToJson(buffer,profile);
	
	EXPECT_EQ(CST_STRING_2,buffer.str());
}

/*******************  FUNCTION  *********************/
TEST(ProfiledValue,push_more_than_max_1)
{
	//setup
	ProfiledStateMaxInt profile(10);
	
	//fill
	ticks t = 0;
	for (int i = 0 ; i < 20 ; i++)
	{
		profile.push(t++,i,CST_FUNC_1);
		profile.push(t++,i,CST_FUNC_2);
	}
	
	//output
	std::stringstream buffer;
	profile.flush();
	htopml::convertToJson(buffer,profile);
	
	EXPECT_EQ(CST_STRING_3,buffer.str());
}

/*******************  FUNCTION  *********************/
TEST(ProfiledValue,push_more_than_max_2)
{
	//setup
	ProfiledStateMaxInt profile(10);
	
	//fill
	ticks t = 0;
	for (int i = 0 ; i < 20 ; i++)
	{
		profile.push(t++,2*i,CST_FUNC_1);
		profile.push(t++,2*i+1,CST_FUNC_2);
	}
	
	//output
	std::stringstream buffer;
	profile.flush();
	htopml::convertToJson(buffer,profile);
	
	EXPECT_EQ(CST_STRING_4,buffer.str());
}

/*******************  FUNCTION  *********************/
TEST(ProfiledValue,hole)
{
	//setup
	ProfiledStateMaxInt profile(10);
	
	//fill
	ticks t = 0;
	profile.push(t++,1,CST_FUNC_1);
	profile.push(t++,1,CST_FUNC_1);
	t+=2;
	profile.push(t++,3,CST_FUNC_1);
	profile.push(t++,3,CST_FUNC_1);
	profile.push(t++,4,CST_FUNC_1);
	
	//output
	std::stringstream buffer;
	profile.flush();
	htopml::convertToJson(buffer,profile);
	
	EXPECT_EQ(CST_STRING_5,buffer.str());
}

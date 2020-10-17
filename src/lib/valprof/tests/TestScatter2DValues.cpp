/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <sstream>
#include <cstdio>
#include <gtest/gtest.h>
#include <json/ConvertToJson.h>
#include <valprof/Scatter2DValues.hpp>

/***************** USING NAMESPACE ******************/
using namespace MALT;

/*******************  FUNCTION  *********************/
void loadFile(std::string & value,const char * path)
{
	//reset
	value.clear();
	
	//open
	FILE * fp = fopen(path,"r");
	ASSERT_NE((FILE*)NULL,fp);
	
	//read
	char buffer[1025];
	while (!feof(fp))
	{
		size_t size = fread(buffer,1,sizeof(buffer)-1,fp);
		if (size > 0)
		{
			buffer[size] = '\0';
			value += buffer;
		}
	}
	
	fclose(fp);
}

/*******************  FUNCTION  *********************/
TEST(Scatter2DValues,constructor)
{
	Scatter2DValues scatter(1024,1024,false,false);
}

/*******************  FUNCTION  *********************/
TEST(Scatter2DValues,simple)
{
	Scatter2DValues scatter(1024,1024,false,false);
	
	//fill
	scatter.push(10,20);
	scatter.push(10,20);
	scatter.push(512,256);
	
	//output
	std::string ref;
	loadFile(ref,TEST_DATA_PATH "/Scatter2DValues-simple.json");
	std::stringstream buffer;
	htopml::convertToJson(buffer,scatter);
	
	EXPECT_EQ(ref,buffer.str());
}

/*******************  FUNCTION  *********************/
TEST(Scatter2DValues,limit)
{
	Scatter2DValues scatter(1024,1024,false,false);
	
	//fill
	scatter.push(1024,1024);
	
	//output
	std::string ref;
	loadFile(ref,TEST_DATA_PATH "/Scatter2DValues-limit.json");
	std::stringstream buffer;
	htopml::convertToJson(buffer,scatter);
	
	EXPECT_EQ(ref,buffer.str());
}

/*******************  FUNCTION  *********************/
TEST(Scatter2DValues,simpleScaleX)
{
	Scatter2DValues scatter(1024,1024,false,false);
	
	//fill
	scatter.push(10,20);
	scatter.push(10,20);
	scatter.push(1524,256);
	
	//output
	std::string ref;
	loadFile(ref,TEST_DATA_PATH "/Scatter2DValues-simpleScaleX.json");
	std::stringstream buffer;
	htopml::convertToJson(buffer,scatter);
	
	EXPECT_EQ(ref,buffer.str());
}

/*******************  FUNCTION  *********************/
TEST(Scatter2DValues,simpleScaleXY)
{
	Scatter2DValues scatter(1024,1024,false,false);
	
	//fill
	scatter.push(9,19);
	scatter.push(9,19);
	scatter.push(1525,1525);
	
	//output
	std::string ref;
	loadFile(ref,TEST_DATA_PATH "/Scatter2DValues-simpleScaleXY.json");
	std::stringstream buffer;
	htopml::convertToJson(buffer,scatter);
	
	EXPECT_EQ(ref,buffer.str());
}

/*******************  FUNCTION  *********************/
TEST(Scatter2DValues,logSimple)
{
	Scatter2DValues scatter(1024,1024,true,true);
	
	//fill
	scatter.push(1,2);
	scatter.push(4,256);
	scatter.push(512,1023);
	
	//output
	std::string ref;
	loadFile(ref,TEST_DATA_PATH "/Scatter2DValues-logSimple.json");
	std::stringstream buffer;
	htopml::convertToJson(buffer,scatter);
	
	EXPECT_EQ(ref,buffer.str());
}

/*******************  FUNCTION  *********************/
TEST(Scatter2DValues,logScaleXZ)
{
	Scatter2DValues scatter(1024,1024,true,true);
	
	//fill
	scatter.push(1,1);
	scatter.push(128,128);
	scatter.push(256,256);
	scatter.push(1024,1024);
	scatter.push(2047,2047);
	
	//output
	std::string ref;
	loadFile(ref,TEST_DATA_PATH "/Scatter2DValues-logScaleXZ.json");
	std::stringstream buffer;
	htopml::convertToJson(buffer,scatter);
	
	EXPECT_EQ(ref,buffer.str());
}

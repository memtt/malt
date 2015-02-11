/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <sstream>
#include <json/ConvertToJson.h>
#include <core/Options.hpp>

/***************** USING NAMESPACE ******************/
using namespace MATT;

/********************** CONSTS **********************/
static const char * CST_VALUE_1 = "{\n\t\"stack\":{\n\t\t\"enabled\":true,\n\t\t\"resolve\":true,\n\t\t\"mode\":\"backtrace\",\n\t\t\"libunwind\":false\n\t},\n\t\"time\":{\n\t\t\"enabled\":true,\n\t\t\"points\":512,\n\t\t\"linear\":false\n\t},\n\t\"output\":{\n\t\t\"name\":\"matt-%1-%2.%3\",\n\t\t\"indent\":true,\n\t\t\"json\":true,\n\t\t\"lua\":false,\n\t\t\"callgrind\":false,\n\t\t\"config\":true\n\t},\n\t\"max-stack\":{\n\t\t\"enabled\":true\n\t},\n\t\"distr\":{\n\t\t\"alloc_size\":true,\n\t\t\"realloc_jump\":true\n\t},\n\t\"trace\":{\n\t\t\"enabled\":true\n\t},\n\t\"info\":{\n\t\t\"hidden\":false\n\t}\n}";
;

/*******************  FUNCTION  *********************/
TEST(TestOptions,constructor)
{
	Options options;
}

/*******************  FUNCTION  *********************/
TEST(TestOptions,save)
{
	Options options;
	options.dumpConfig("test-config-dump.ini");
}

/*******************  FUNCTION  *********************/
TEST(TestOptions,saveAndLoad)
{
	Options options;
	options.dumpConfig("test-config-load.ini");
	
	Options options2;
	options2.loadFromFile("test-config-load.ini");
	
	EXPECT_EQ(options,options2);
}

/*******************  FUNCTION  *********************/
TEST(TestOptions,dumpJson)
{
	std::stringstream out;
	Options option;
	htopml::convertToJson(out,option);
	EXPECT_EQ(CST_VALUE_1,out.str());
}

/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <json/JsonState.h>
#include "Options.hpp"

/***************** USING NAMESPACE ******************/
using namespace MALT;

/********************  CONST  ***********************/
const char cstJson[] = "\
{\n\
	\"time\":{\n\
		\"enabled\":true,\n\
		\"points\":512,\n\
		\"linear\":false\n\
	},\n\
	\"stack\":{\n\
		\"enabled\":true,\n\
		\"mode\":\"backtrace\",\n\
		\"resolve\":true,\n\
		\"libunwind\":false,\n\
		\"stackSkip\":4\n\
	},\n\
	\"output\":{\n\
		\"callgrind\":false,\n\
		\"dumpConfig\":false,\n\
		\"index\":false,\n\
		\"json\":true,\n\
		\"lua\":false,\n\
		\"name\":\"malt-%1-%2.%3\",\n\
		\"verbosity\":\"default\",\n\
		\"stackTree\":false,\n\
		\"loopSuppress\":false\n\
	},\n\
	\"maxStack\":{\n\
		\"enabled\":true\n\
	},\n\
	\"distr\":{\n\
		\"allocSize\":true,\n\
		\"reallocJump\":true\n\
	},\n\
	\"info\":{\n\
		\"hidden\":false\n\
	},\n\
	\"filter\":{\n\
		\"exe\":\"\",\n\
		\"childs\":true,\n\
		\"enabled\":true\n\
	},\n\
	\"dump\":{\n\
		\"onSignal\":\"\",\n\
		\"afterSeconds\":0\n\
	}\n\
}";

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
TEST(TestOptions,loadFromString)
{
	//build
	Options options;

	//pre check
	EXPECT_FALSE(options.traceEnabled);
	EXPECT_FALSE(options.outputIndent);

	//parse
	options.loadFromString("output:indent=true;output:name=test;trace:enabled=true)");

	//recheck
	EXPECT_TRUE(options.traceEnabled);
	EXPECT_TRUE(options.outputIndent);
	EXPECT_EQ(options.outputName, "test");
}

/*******************  FUNCTION  *********************/
TEST(TestOptions,toSJon)
{
	//build
	Options options;
	std::stringstream out;
	htopml::convertToJson(out, options, true);
	EXPECT_EQ(out.str(), cstJson);
}

/*******************  FUNCTION  *********************/
TEST(TestVerbosity,ostream)
{
	std::stringstream out;
	out << Verbosity::MALT_VERBOSITY_DEFAULT << ",";
	out << Verbosity::MALT_VERBOSITY_SILENT << ",";
	out << Verbosity::MALT_VERBOSITY_VERBOSE;
	EXPECT_EQ(out.str(), "default,silent,verbose");
}

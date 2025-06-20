/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 06/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/common/tests/TestOptions.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2022
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include <json/JsonState.h>
#include "Options.hpp"

/**********************************************************/
using namespace MALT;

/**********************************************************/
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
		\"stackSkip\":4,\n\
		\"addr2lineBucket\":350,\n\
		\"addr2lineThreads\":8,\n\
		\"sampling\":false,\n\
		\"samplingBw\":4093\n\
	},\n\
	\"python\":{\n\
		\"instru\":true,\n\
		\"mix\":false,\n\
		\"stack\":\"enter-exit\",\n\
		\"obj\":true,\n\
		\"mem\":true,\n\
		\"raw\":true,\n\
		\"hideImports\":true\n\
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
		\"afterSeconds\":0,\n\
		\"onSysFullAt\":\"\",\n\
		\"onAppUsingRss\":\"\",\n\
		\"onAppUsingVirt\":\"\",\n\
		\"onAppUsingReq\":\"\",\n\
		\"onThreadStackUsing\":\"\",\n\
		\"onAllocCount\":\"\",\n\
		\"watchDog\":false\n\
	},\n\
	\"tools\":{\n\
		\"nm\":true,\n\
		\"nmMaxSize\":\"50M\"\n\
	}\n\
}";

/**********************************************************/
TEST(TestOptions,constructor)
{
	Options options;
}

/**********************************************************/
TEST(TestOptions,save)
{
	Options options;
	options.dumpConfig("test-config-dump.ini");
}

/**********************************************************/
TEST(TestOptions,saveAndLoad)
{
	Options options;
	options.dumpConfig("test-config-load.ini");
	
	Options options2;
	options2.loadFromFile("test-config-load.ini");
	
	EXPECT_EQ(options,options2);
}

/**********************************************************/
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

/**********************************************************/
TEST(TestOptions,toSJon)
{
	//build
	Options options;
	std::stringstream out;
	htopml::convertToJson(out, options, true);
	EXPECT_EQ(cstJson, out.str());
}

/**********************************************************/
TEST(TestVerbosity,ostream)
{
	std::stringstream out;
	out << Verbosity::MALT_VERBOSITY_DEFAULT << ",";
	out << Verbosity::MALT_VERBOSITY_SILENT << ",";
	out << Verbosity::MALT_VERBOSITY_VERBOSE;
	EXPECT_EQ(out.str(), "default,silent,verbose");
}

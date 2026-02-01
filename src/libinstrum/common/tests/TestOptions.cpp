/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/common/tests/TestOptions.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2026
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
\t\"addr2line\":{\n\
\t\t\"bucket\":350,\n\
\t\t\"huge\":52428800,\n\
\t\t\"threads\":8\n\
\t},\n\
\t\"c\":{\n\
\t\t\"gpu-malloc\":true,\n\
\t\t\"malloc\":true,\n\
\t\t\"mmap\":true\n\
\t},\n\
\t\"distr\":{\n\
\t\t\"alloc-size\":true,\n\
\t\t\"realloc-jump\":true\n\
\t},\n\
\t\"dump\":{\n\
\t\t\"after-seconds\":0,\n\
\t\t\"on-alloc-count\":\"\",\n\
\t\t\"on-app-using-req\":\"\",\n\
\t\t\"on-app-using-rss\":\"\",\n\
\t\t\"on-app-using-virt\":\"\",\n\
\t\t\"on-signal\":\"\",\n\
\t\t\"on-sys-full-at\":\"\",\n\
\t\t\"on-thread-stack-using\":\"\",\n\
\t\t\"watch-dog\":false\n\
\t},\n\
\t\"filter\":{\n\
\t\t\"childs\":true,\n\
\t\t\"enabled\":true,\n\
\t\t\"exe\":\"\",\n\
\t\t\"ranks\":\"\"\n\
\t},\n\
\t\"info\":{\n\
\t\t\"hidden\":false\n\
\t},\n\
\t\"max-stack\":{\n\
\t\t\"enabled\":true\n\
\t},\n\
\t\"output\":{\n\
\t\t\"callgrind\":false,\n\
\t\t\"config\":false,\n\
\t\t\"indent\":false,\n\
\t\t\"json\":true,\n\
\t\t\"loop-suppress\":true,\n\
\t\t\"lua\":false,\n\
\t\t\"name\":\"malt-%1-%2.%3\",\n\
\t\t\"stack-tree\":false,\n\
\t\t\"verbosity\":\"default\"\n\
\t},\n\
\t\"python\":{\n\
\t\t\"hide-imports\":true,\n\
\t\t\"instru\":true,\n\
\t\t\"mem\":true,\n\
\t\t\"mix\":false,\n\
\t\t\"mode\":\"profile\",\n\
\t\t\"obj\":true,\n\
\t\t\"raw\":true,\n\
\t\t\"stack\":\"enter-exit\"\n\
\t},\n\
\t\"sampling\":{\n\
\t\t\"count\":571,\n\
\t\t\"enabled\":false,\n\
\t\t\"volume\":4093\n\
\t},\n\
\t\"stack\":{\n\
\t\t\"enabled\":true,\n\
\t\t\"libunwind\":false,\n\
\t\t\"mode\":\"backtrace\",\n\
\t\t\"resolve\":true,\n\
\t\t\"skip\":4\n\
\t},\n\
\t\"time\":{\n\
\t\t\"enabled\":true,\n\
\t\t\"linear\":false,\n\
\t\t\"points\":512\n\
\t},\n\
\t\"tools\":{\n\
\t\t\"nm\":true,\n\
\t\t\"nm-max-size\":\"50M\"\n\
\t},\n\
\t\"trace\":{\n\
\t\t\"enabled\":false\n\
\t}\n\
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
	EXPECT_FALSE(options.trace.enabled);
	EXPECT_FALSE(options.output.indent);

	//parse
	options.loadFromString("output:indent=true;output:name=test;trace:enabled=true)");

	//recheck
	EXPECT_TRUE(options.trace.enabled);
	EXPECT_TRUE(options.output.indent);
	EXPECT_EQ(options.output.name, "test");
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

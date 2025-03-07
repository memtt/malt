/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/common/tests/TestDebug.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2022 - 2024
*    AUTHOR   : Sébastien Valat (INRIA) - 2023 - 2024
***********************************************************/

/**********************************************************/
#include <sstream>
#include <gtest/gtest.h>
#include "Debug.hpp"

/**********************************************************/
using namespace MALT;

/**********************************************************/
#ifndef NDEBUG
TEST(Debug,end_not_called)
{
	Debug * debug = new Debug("message", "source.cpp", 10);
	delete debug;
}
#endif

/**********************************************************/
TEST(Debug,debug)
{
	Debug debug("message %1", "source.cpp", 10, MESSAGE_DEBUG);
	debug.arg("value");
	std::stringstream out;
	std::stringstream err;
	debug.end(out, err);
	#ifndef NDEBUG
		EXPECT_EQ("\nDebug : At source.cpp:10 : \nDebug : message value\n", out.str());
	#else
		EXPECT_EQ("", out.str());
	#endif
	EXPECT_EQ("", err.str());
}

/**********************************************************/
TEST(Debug,info)
{
	Debug debug("message %1", "source.cpp", 10, MESSAGE_INFO);
	debug.arg("value");
	std::stringstream out;
	std::stringstream err;
	debug.end(out, err);
	EXPECT_EQ("\nInfo : At source.cpp:10 : \nInfo : message value\n", out.str());
	EXPECT_EQ("", err.str());
}

/**********************************************************/
TEST(Debug,info_no_line)
{
	Debug debug("message %1", MESSAGE_INFO);
	debug.arg("value");
	std::stringstream out;
	std::stringstream err;
	debug.end(out, err);
	EXPECT_EQ("Info : message value\n", out.str());
	EXPECT_EQ("", err.str());
}

/**********************************************************/
TEST(Debug,normal)
{
	Debug debug("message %1", "source.cpp", 10, MESSAGE_NORMAL);
	debug.arg("value");
	std::stringstream out;
	std::stringstream err;
	debug.end(out, err);
	EXPECT_EQ("\nAt source.cpp:10 : \nmessage value\n", out.str());
	EXPECT_EQ("", err.str());
}

/**********************************************************/
TEST(Debug,assertion)
{
	Debug debug("message %1", "source.cpp", 10, MESSAGE_ASSERT);
	debug.arg("value");
	debug.enableFakeAbort();
	std::stringstream out;
	std::stringstream err;
	debug.end(out, err);
	#ifndef NDEBUG
		EXPECT_EQ("", out.str());
		EXPECT_EQ("\nAssert : At source.cpp:10 : \nAssert : message value\n", err.str());
		EXPECT_TRUE(debug.aborted());
	#else
		EXPECT_EQ("", out.str());
		EXPECT_EQ("", err.str());
	#endif
}

/**********************************************************/
TEST(Debug,warning)
{
	Debug debug("message %1", "source.cpp", 10, MESSAGE_WARNING);
	debug.arg("value");
	std::stringstream out;
	std::stringstream err;
	debug.end(out, err);
	EXPECT_EQ("", out.str());
	EXPECT_EQ("\nWarning : At source.cpp:10 : \nWarning : message value\n", err.str());
}

/**********************************************************/
TEST(Debug,error)
{
	Debug debug("message %1", "source.cpp", 10, MESSAGE_ERROR);
	debug.arg("value");
	std::stringstream out;
	std::stringstream err;
	debug.end(out, err);
	EXPECT_EQ("", out.str());
	EXPECT_EQ("\nError : At source.cpp:10 : \nError : message value\n", err.str());
}

/**********************************************************/
TEST(Debug,fatal)
{
	Debug debug("message %1", "source.cpp", 10, MESSAGE_FATAL);
	debug.arg("value");
	debug.enableFakeAbort();
	std::stringstream out;
	std::stringstream err;
	debug.end(out, err);
	EXPECT_EQ("", out.str());
	EXPECT_EQ("\nFatal : At source.cpp:10 : \nFatal : message value\n", err.str());
	EXPECT_TRUE(debug.aborted());
}

/**********************************************************/
TEST(Debug,macros)
{
	//basic
	ASSERT_DEATH(MALT_FATAL("test"), ".*test.*");
	MALT_DEBUG("ut", "test");
	MALT_ERROR("test");
	MALT_WARNING("test");
	MALT_MESSAGE("test");
	MALT_INFO("test");

	//advanced
	MALT_FATAL_ARG("test %1").enableFakeAbort().arg("value").end();
	MALT_DEBUG_ARG("ut", "test %1").arg("value").end();
	MALT_ERROR_ARG("test %1").arg("value").end();
	MALT_WARNING_ARG("test %1").arg("value").end();
	MALT_MESSAGE_ARG("test %1").arg("value").end();
	MALT_INFO_ARG("test %1").arg("value").end();
}

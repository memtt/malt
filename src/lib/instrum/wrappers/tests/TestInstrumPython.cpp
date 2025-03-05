/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/wrappers/tests/TestInstrum.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include <thread>
#include <core/ThreadTracker.hpp>
#include "portability/Python.hpp"
#include "../WrapperCAlloc.hpp"
#include "state/GlobalState.hpp"
#include "injectors/InjectPythonInit.hpp"
#include "state/malt.h"
#include <common/Helpers.hpp>

/**********************************************************/
using namespace MALT;

/**********************************************************/
int maltInitStatus(void)
{
	return 0;
}

/**********************************************************/
TEST(TestInstrum, python_basic_array_backtrace)
{
	//reset MALT & enable
	const std::string profile = BINARY_DIR "/malt-python-basic-array.json";
	setenv("MALT_OPTIONS", "stack:mode=backtrace;python:mix=false;output:name=" BINARY_DIR "/malt-python-basic-array.json", 1);
	MALT::globalResetForTests();

	//make an alloc
	maltEnable();
	const char * argv[] = {"TestInstrum", CUR_SRC_DIR "/main1.py", NULL};
	int status = Py_BytesMain(2, (char**)argv);
	maltDisable();

	//check
	ASSERT_EQ(status, 0);

	//dump & get
	MALT::gblState.onExit();

	//load ref
	//TODO: this is a bad trick, I should avoid extracting values of main() which vary due to python version.
	std::string ref1 = MALT::Helpers::loadFullFile(SRC_DIR "/src/lib/insturm/wrappers/tests/ref-pyhton-basic-array-backtrace.txt");

	//conver
	std::string result = MALT::Helpers::simpleProfileDump(profile, CUR_SRC_DIR "/main1.py", false);

	//save ref
	//MALT::Helpers::writeFullFile(SRC_DIR "/src/lib/insturm/wrappers/tests/ref-pyhton-basic-array-backtrace.txt", result);

	//check
	ASSERT_EQ(ref1, result);
}

/**********************************************************/
TEST(TestInstrum, python_basic_array_enter_exit)
{
	//reset MALT & enable
	const std::string profile = BINARY_DIR "/malt-python-basic-array.json";
	setenv("MALT_OPTIONS", "stack:mode=enter-exit;python:stack=enter-exit;python:mix=false;output:name=" BINARY_DIR "/malt-python-basic-array.json", 1);
	MALT::globalResetForTests();

	//make an alloc
	maltEnable();
	const char * argv[] = {"TestInstrum", CUR_SRC_DIR "/main1.py", NULL};
	int status = Py_BytesMain(2, (char**)argv);
	maltDisable();

	//check
	ASSERT_EQ(status, 0);

	//dump & get
	MALT::gblState.onExit();

	//load ref
	//TODO: this is a bad trick, I should avoid extracting values of main() which vary due to python version.
	std::string ref1 = MALT::Helpers::loadFullFile(SRC_DIR "/src/lib/insturm/wrappers/tests/ref-pyhton-basic-array.txt");
	
	//conver
	std::string result = MALT::Helpers::simpleProfileDump(profile, CUR_SRC_DIR "/main1.py", false);

	//save ref
	//MALT::Helpers::writeFullFile(SRC_DIR "/src/lib/insturm/wrappers/tests/ref-pyhton-basic-array.txt", result);

	//check
	ASSERT_EQ(ref1, result);
}

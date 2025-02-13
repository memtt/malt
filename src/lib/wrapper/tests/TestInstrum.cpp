/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/wrapper/tests/TestThreadTracker.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2022 - 2024
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include <thread>
#include <ThreadTracker.hpp>
#include "../WrapperCAlloc.hpp"
#include "../GlobalState.hpp"
#include "../malt.h"
#include <common/Helpers.hpp>
#include <Python.h>

/**********************************************************/
using namespace MALT;

/**********************************************************/
int maltInitStatus(void)
{
	return 0;
}

/**********************************************************/
void * function_testing_c_malloc()
{
	return malloc(16);
}

/**********************************************************/
TEST(TestInstrum, c_basic_malloc)
{
	//reset MALT & enable
	const std::string profile = BINARY_DIR "/malt-c-basic-malloc.json";
	setenv("MALT_OPTIONS", "output:name=" BINARY_DIR "/malt-c-basic-malloc.json", 1);
	MALT::globalResetForTests();

	//make an alloc
	maltEnable();
	void * ptr = function_testing_c_malloc();
	maltDisable();

	//check
	ASSERT_NE(ptr, nullptr);

	//dump & get
	MALT::gblState.onExit();

	//load ref
	std::string ref = MALT::Helpers::loadFullFile(SRC_DIR "/src/lib/wrapper/tests/ref-c-basic-malloc.txt");

	//conver
	std::string result = MALT::Helpers::simpleProfileDump(profile, SRC_DIR "/src/lib/wrapper/tests/TestInstrum.cpp");

	//MALT::Helpers::writeFullFile(SRC_DIR "/src/lib/wrapper/tests/ref-c-basic-malloc.txt", result);
	ASSERT_EQ(ref, result);
}

/**********************************************************/
namespace MALT
{
	void initPythonAllocInstrumentation();
	typedef int (*Py_RunMainFuncPtr) (void);
}

/**********************************************************/
int Py_RunMain()
{
	Py_RunMainFuncPtr realPy_RunMain = (MALT::Py_RunMainFuncPtr)dlsym(RTLD_NEXT,"Py_RunMain");
	MALT::initPythonAllocInstrumentation();
	return realPy_RunMain();
}

/**********************************************************/
TEST(TestInstrum, python_basic_array)
{
	//reset MALT & enable
	const std::string profile = BINARY_DIR "/malt-python-basic-array.json";
	setenv("MALT_OPTIONS", "output:name=" BINARY_DIR "/malt-python-basic-array.json", 1);
	MALT::globalResetForTests();

	//make an alloc
	maltEnable();
	const char * argv[] = {"TestInstrum", SRC_DIR "/src/lib/wrapper/tests/main1.py", NULL};
	int status = Py_BytesMain(2, (char**)argv);
	maltDisable();

	//check
	ASSERT_EQ(status, 0);

	//dump & get
	MALT::gblState.onExit();

	//load ref
	std::string ref = MALT::Helpers::loadFullFile(SRC_DIR "/src/lib/wrapper/tests/ref-pyhton-basic-array.txt");

	//conver
	std::string result = MALT::Helpers::simpleProfileDump(profile, SRC_DIR "/src/lib/wrapper/tests/main1.py");

	//MALT::Helpers::writeFullFile(SRC_DIR "/src/lib/wrapper/tests/ref-pyhton-basic-array.txt", result);
	ASSERT_EQ(ref, result);
}

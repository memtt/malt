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
#include <Python.h>
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
	std::string ref = MALT::Helpers::loadFullFile(SRC_DIR "/src/lib/wrappers/tests/ref-c-basic-malloc.txt");

	//conver
	std::string result = MALT::Helpers::simpleProfileDump(profile, SRC_DIR "/src/lib/wrappers/tests/TestInstrum.cpp");

	//MALT::Helpers::writeFullFile(SRC_DIR "/src/lib/wrappers/tests/ref-c-basic-malloc.txt", result);
	ASSERT_EQ(ref, result);
}

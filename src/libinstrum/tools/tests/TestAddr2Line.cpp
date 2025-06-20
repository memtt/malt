/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/tools/tests/TestAddr2Line.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include <portability/OS.hpp>
#include <common/STLInternalAllocator.hpp>
#include "../Addr2Line.hpp"

/**********************************************************/
using namespace MALT;

/**********************************************************/
const int cstLineA = __LINE__;
static void testFunctionA(void)
{
	return;
}

/**********************************************************/
const int cstLineB = __LINE__;
static void testFunctionB(void)
{
	return;
}

/**********************************************************/
TEST(TestAddr2Line,constructor)
{
	//vars
	StringIdDictionnary dict;

	//get offset
	size_t aslrOffset = OS::getASLROffset((void*)testFunctionA);

	//build
	Addr2Line addr2line(dict, TEST_BIN_DIR "/TestAddr2Line", aslrOffset, 8);

	//reg two funcs
	CallSite siteA;
	addr2line.addTask(LangAddress(DOMAIN_C, (void*)testFunctionA), &siteA);
	CallSite siteB;
	addr2line.addTask(LangAddress(DOMAIN_C, (void*)testFunctionB), &siteB);

	//check
	bool status = addr2line.run();
	ASSERT_TRUE(status);

	//check
	ASSERT_EQ(dict.getString(siteA.function), "testFunctionA()");
	ASSERT_EQ(siteA.line, cstLineA + 2);
	ASSERT_EQ(dict.getString(siteB.function), "testFunctionB()");
	ASSERT_EQ(siteB.line, cstLineB + 2);
}

/**********************************************************/
int main(int argc, char ** argv)
{
	//init internal allocator
	gblInternaAlloc = new SimpleAllocator(true);
	
	// This allows the user to override the flag on the command line.
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

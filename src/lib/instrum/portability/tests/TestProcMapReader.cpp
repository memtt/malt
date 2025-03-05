/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/portability/tests/TestProcMapReader.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2022 - 2024
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include "LinuxProcMapReader.hpp"

/**********************************************************/
using namespace MALT;
using namespace testing;

/**********************************************************/
void localFunc(void)
{
}

/**********************************************************/
TEST(LinuxProcMapReader,load)
{
	LinuxProcMapReader reader;
	reader.load();
}

/**********************************************************/
TEST(LinuxProcMapReader,getEntry)
{
	LinuxProcMapReader reader;
	reader.load();
	const LinuxProcMapEntry *  entry = reader.getEntry((void*)localFunc);
	ASSERT_NE(nullptr, entry);
	EXPECT_STREQ(basename(entry->file.c_str()), "TestProcMapReader");
}

/**********************************************************/
TEST(LinuxProcMapReader,begin_end)
{
	LinuxProcMapReader reader;
	reader.load();
	for (auto & it : reader)
		EXPECT_NE(nullptr, it.lower);
}
/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include "LinuxProcMapReader.hpp"

/***************** USING NAMESPACE ******************/
using namespace MALT;
using namespace testing;

/*******************  FUNCTION  *********************/
void localFunc(void)
{
}

/*******************  FUNCTION  *********************/
TEST(LinuxProcMapReader,load)
{
	LinuxProcMapReader reader;
	reader.load();
}

/*******************  FUNCTION  *********************/
TEST(LinuxProcMapReader,getEntry)
{
	LinuxProcMapReader reader;
	reader.load();
	const LinuxProcMapEntry *  entry = reader.getEntry((void*)localFunc);
	ASSERT_NE(nullptr, entry);
	EXPECT_STREQ(basename(entry->file.c_str()), "TestProcMapReader");
}

/*******************  FUNCTION  *********************/
TEST(LinuxProcMapReader,begin_end)
{
	LinuxProcMapReader reader;
	reader.load();
	for (auto & it : reader)
		EXPECT_NE(nullptr, it.lower);
}
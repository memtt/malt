/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 09/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/common/tests/TestCodeTiming.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2022 - 2024
***********************************************************/

/**********************************************************/
#include <sstream>
#include <gtest/gtest.h>
#include "CodeTiming.hpp"

/**********************************************************/
using namespace MALT;

/**********************************************************/
static ticks gblTicks = 0;

/**********************************************************/
ticks MALT::mockableGetTicks(void)
{
	return gblTicks;
}

/**********************************************************/
TEST(CodeTiming,constructor)
{
	CodeTiming timing("test");
	EXPECT_EQ(timing.getName(), "test");
}

/**********************************************************/
TEST(CodeTiming,cycle)
{
	CodeTiming timing("test");
	ticks start = timing.start();
	usleep(100);
	timing.end(start);
}

/**********************************************************/
TEST(CodeTiming,printValue)
{
	gblTicks = 1000;
	CodeTiming timing("test");

	//two cycles
	timing.end(1000, 2000);
	timing.end(1000, 2000);
	ticksPerSecond(1000);
	
	//convert
	gblTicks = 4000;
	std::stringstream stream;
	timing.finalPrint(stream, true);
	EXPECT_EQ(stream.str(), "TIMING OF test                             => [   1.0 K ,   1.0 K ,   1.0 K ] => TOTAL ( calls :   2.0   , time :   2.0 K , ratio : ~ 50      % )\n");
}

/**********************************************************/
TEST(CodeTiming,printAll)
{
	gblTicks = 1000;

	CodeTiming timing("test1", true);
	timing.end(1000, 2000);
	timing.end(2000, 3000);

	CodeTiming timing2("test2", true);
	timing2.end(1000, 2000);
	timing2.end(2000, 3000);

	ticksPerSecond(1000);

	//convert
	gblTicks = 6000;
	std::stringstream stream;
	timing.printAll(stream, true);
	EXPECT_EQ(stream.str(), 
		"=============================================================== MALT TIMINGS ====================================================================\n"
		"TIMING OF test1                            => [   1.0 K ,   1.0 K ,   1.0 K ] => TOTAL ( calls :   2.0   , time :   2.0 K , ratio : ~ 40      % )\n"
		"TIMING OF test2                            => [   1.0 K ,   1.0 K ,   1.0 K ] => TOTAL ( calls :   2.0   , time :   2.0 K , ratio : ~ 40      % )\n"
		"=================================================================================================================================================\n");
}

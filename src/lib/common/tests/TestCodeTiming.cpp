/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <sstream>
#include <gtest/gtest.h>
#include "CodeTiming.hpp"

/***************** USING NAMESPACE ******************/
using namespace MALT;

/*******************  GLOBALS  **********************/
static ticks gblTicks = 0;

/*******************  FUNCTION  *********************/
ticks MALT::mockableGetTicks(void)
{
	return gblTicks;
}

/*******************  FUNCTION  *********************/
TEST(CodeTiming,constructor)
{
	CodeTiming timing("test");
	EXPECT_EQ(timing.getName(), "test");
}

/*******************  FUNCTION  *********************/
TEST(CodeTiming,cycle)
{
	CodeTiming timing("test");
	ticks start = timing.start();
	usleep(100);
	timing.end(start);
}

/*******************  FUNCTION  *********************/
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

/*******************  FUNCTION  *********************/
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

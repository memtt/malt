/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/core/tests/TestSegmentTracker.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include <core/StackSizeAnalyser.hpp>
#include <core/SymbolSolver.hpp>
#include <portability/Clock.hpp>

/**********************************************************/
using namespace MALT;

/**********************************************************/
const char * CST_REF_1 = "{\n\
\t\"size\":0,\n\
\t\"stack\":[],\n\
\t\"mem\":[0],\n\
\t\"total\":0,\n\
\t\"timeprofiler\":{\n\
\t\t\"min\":[0, 0, 0, 0],\n\
\t\t\"max\":[0, 0, 0, 0],\n\
\t\t\"index\":[1, 2, 3, 4],\n\
\t\t\"location\":[\"0x200\", \"0x200\", \"0x300\", \"0x300\"],\n\
\t\t\"timestamp\":[2000, 3000, 4000, 5000],\n\
\t\t\"peakTimesteamp\":0,\n\
\t\t\"peakMemory\":0,\n\
\t\t\"peakIndex\":0,\n\
\t\t\"linearIndex\":true\n\
\t}\n\
}";

const char * CST_REF_2 = "{\n\
\t\"map\":[],\n\
\t\"strings\":[\"??\"],\n\
\t\"instr\":{\n\
\t\t\"0x1\":{\n\
\n\
\t\t},\n\
\t\t\"0x100\":{\n\
\n\t\t},\n\
\t\t\"0x300\":{\n\
\n\
\t\t}\n\
\t}\n\
}";

/**********************************************************/
//for pre init of global allocator
class ForceInit
{
	public:
		ForceInit(void) {gblInternaAlloc = new SimpleAllocator(true);};
};
ForceInit gblForceInit;

/**********************************************************/
TEST(TestStackSizeAnalyser, constructor)
{
	//build it
	StackSizeAnalyser analyser;
}

/**********************************************************/
TEST(TestStackSizeAnalyser, enter_exit)
{
	//build it
	StackSizeAnalyser analyser;
	analyser.onEnterFunc(LangAddress(DOMAIN_C, (void*)0x100));
	analyser.onExitFunc(LangAddress(DOMAIN_C, (void*)0x100));
}

/**********************************************************/
TEST(TestStackSizeAnalyser, full_workflow)
{
	//reset clock
	Clock::enableLineratTimingsForTests();

	//build it
	StackSizeAnalyser analyser;

	//play
	analyser.onEnterFunc(LangAddress(DOMAIN_C, (void*)0x100));
	analyser.onEnterFunc(LangAddress(DOMAIN_C, (void*)0x200));
	analyser.onExitFunc(LangAddress(DOMAIN_C, (void*)0x200));
	analyser.onEnterFunc(LangAddress(DOMAIN_C, (void*)0x300));
	analyser.onExitFunc(LangAddress(DOMAIN_C, (void*)0x300));
	analyser.onExitFunc(LangAddress(DOMAIN_C, (void*)0x100));

	//solver
	SymbolSolver solver;
	solver.registerFunctionSymbol((void*)0x100, "TestStackSizeAnalyser", "main()", "program", 10);
	solver.registerFunctionSymbol((void*)0x200, "TestStackSizeAnalyser", "functionA()", "program", 20);
	solver.registerFunctionSymbol((void*)0x300, "TestStackSizeAnalyser", "functionB()", "program", 30);

	//to json
	std::stringstream out;
	htopml::convertToJson(out, analyser);

	//compare
	ASSERT_EQ(out.str(), CST_REF_1);
}

/**********************************************************/
TEST(TestStackSizeAnalyser, solveSymbos)
{
	//reset clock
	Clock::enableLineratTimingsForTests();

	//build it
	StackSizeAnalyser analyser;

	//play
	analyser.onEnterFunc(LangAddress(DOMAIN_C, (void*)0x100), 3000);
	analyser.onEnterFunc(LangAddress(DOMAIN_C, (void*)0x200), 2000);
	analyser.onExitFunc(LangAddress(DOMAIN_C, (void*)0x200));
	analyser.onEnterFunc(LangAddress(DOMAIN_C, (void*)0x300), 1000);
	analyser.onExitFunc(LangAddress(DOMAIN_C, (void*)0x300));
	analyser.onExitFunc(LangAddress(DOMAIN_C, (void*)0x100));

	//solve
	SymbolSolver solver;
	analyser.solveSymbols(solver);

	//convert
	std::stringstream out;
	htopml::convertToJson(out, solver);

	//check
	ASSERT_EQ(out.str(), CST_REF_2);
}

/**********************************************************/
int main(int argc, char ** argv)
{
	//init internal allocator
	gblInternaAlloc = new SimpleAllocator(true);
	
	//ini gblOptions
	initGlobalOptions();
	
	// This allows the user to override the flag on the command line.
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

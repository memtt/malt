/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 06/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/core/tests/TestSymbolSolver.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2019 - 2024
*    AUTHOR   : Bastien Levasseur - 2024
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include <SymbolSolver.hpp>
#include <common/Options.hpp>
#include <common/SimpleAllocator.hpp>
#include <common/GTestHelper.hpp>
#include <stacks/BacktraceStack.hpp>

/**********************************************************/
using namespace MALT;

/**********************************************************/
int main(int argc, char ** argv);

/**********************************************************/
int isFunctionForTest(int argument)
{
	return EXIT_SUCCESS;
}

/**********************************************************/
TEST(TestSymbolSolver,testConstructor)
{
	SymbolSolver solver;
}

/**********************************************************/
TEST(TestSymbolSolver,testLoadProcMap)
{
	SymbolSolver solver;
	solver.loadProcMap();
}

/**********************************************************/
TEST(TestSymbolSolver,testSolve)
{
	SymbolSolver solver;
	solver.loadProcMap();
	solver.registerAddress(LangAddress(DOMAIN_C, (void*)::isFunctionForTest));
	solver.solveNames();

	//extact
	const CallSite * site = solver.getCallSiteInfo(LangAddress(DOMAIN_C, (void*)::isFunctionForTest));
	const String & func = solver.getString(site->function);
	const String & file = solver.getString(site->file);

	//test
	#ifndef NDEBUG
		EXPECT_EQ(__FILE__, file);
		EXPECT_STREQ("isFunctionForTest(int)", func.c_str());
	#endif
}

/**********************************************************/
void testCalleeLib(BacktraceStack & stack);
void testCalleeExe(BacktraceStack & stack)
{
	stack.loadCurrentStack();
}

/**********************************************************/
TEST(TestSymbolSolver, solveBacktraceExe)
{
	//vars
	BacktraceStack stack;
	SymbolSolver solver;

	//skipe delta
	size_t skipDelta = stack.getBactraceSkipOptimDelta();

	//run
	testCalleeExe(stack);
	stack.fastSkip(skipDelta+1);

	//solve
	LangAddress callee = stack.getCallee();
	EXPECT_NE(callee, nullAddr);
	solver.loadProcMap();
	solver.registerAddress(callee);
	solver.solveNames();

	//extact
	const CallSite * site = solver.getCallSiteInfo(callee);
	const std::string & func = solver.getString(site->function).c_str();
	const std::string & file = solver.getString(site->file).c_str();

	//check
	#ifndef NDEBUG
		EXPECT_EQ(__FILE__, file);
		EXPECT_EQ("testCalleeExe(MALT::BacktraceStack&)", func);
		EXPECT_EQ(70, site->line);
	#endif
}

/**********************************************************/
TEST(TestSymbolSolver, solveBacktraceLib)
{
	//vars
	BacktraceStack stack;
	SymbolSolver solver;

	//skipe delta
	size_t skipDelta = stack.getBactraceSkipOptimDelta();

	//run
	testCalleeLib(stack);
	stack.fastSkip(skipDelta+1);

	//extract
	LangAddress callee = stack.getCallee();
	solver.loadProcMap();
	solver.registerAddress(callee);
	solver.solveNames();
	
	//extact
	const CallSite * site = solver.getCallSiteInfo(callee);
	const std::string & func = solver.getString(site->function).c_str();

	//check
	#ifndef NDEBUG
		EXPECT_NE(0, site->file);
		EXPECT_NE(-1, site->file);
		EXPECT_EQ("testCalleeLib(MALT::BacktraceStack&)", func);
		EXPECT_EQ(19, site->line);
	#endif
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

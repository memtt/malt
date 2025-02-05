/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.5
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/core/tests/TestSymbolSolver.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2019 - 2024
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
	solver.registerAddress((void*)main);
	solver.solveNames();

	//extact
	const CallSite * site = solver.getCallSiteInfo((void*)main);
	std::string func = solver.getString(site->function);
	std::string file = solver.getString(site->file);

	//test
	#ifndef NDEBUG
		EXPECT_EQ(__FILE__, file);
		EXPECT_EQ("main", func);
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

	//run
	testCalleeExe(stack);
	stack.fastSkip(3);

	//solve
	void * callee = stack.getCallee();
	solver.loadProcMap();
	solver.registerAddress(callee);
	solver.solveNames();

	//extact
	const CallSite * site = solver.getCallSiteInfo(callee);
	std::string func = solver.getString(site->function);
	std::string file = solver.getString(site->file);

	//check
	#ifndef NDEBUG
		EXPECT_EQ(__FILE__, file);
		EXPECT_EQ("testCalleeExe(MALT::BacktraceStack&)", func);
		EXPECT_EQ(62, site->line);
	#endif
}

/**********************************************************/
TEST(TestSymbolSolver, solveBacktraceLib)
{
	//vars
	BacktraceStack stack;
	SymbolSolver solver;

	//run
	testCalleeLib(stack);
	stack.fastSkip(3);

	//extract
	void * callee = stack.getCallee();
	solver.loadProcMap();
	solver.registerAddress(callee);
	solver.solveNames();
	
	//extact
	const CallSite * site = solver.getCallSiteInfo(callee);
	std::string func = solver.getString(site->function);

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

/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <SymbolSolver.hpp>
#include <common/Options.hpp>
#include <common/SimpleAllocator.hpp>
#include <common/GTestHelper.hpp>
#include <stacks/BacktraceStack.hpp>

/***************** USING NAMESPACE ******************/
using namespace MALT;

/*******************  FUNCTION  *********************/
int main(int argc, char ** argv);

/*******************  FUNCTION  *********************/
TEST(TestSymbolSolver,testConstructor)
{
	SymbolSolver solver;
}

/*******************  FUNCTION  *********************/
TEST(TestSymbolSolver,testLoadProcMap)
{
	SymbolSolver solver;
	solver.loadProcMap();
}

/*******************  FUNCTION  *********************/
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

/*******************  FUNCTION  *********************/
void testCalleeLib(BacktraceStack & stack);
void testCalleeExe(BacktraceStack & stack)
{
	stack.loadCurrentStack();
}

/*******************  FUNCTION  *********************/
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
		EXPECT_EQ(60, site->line);
	#endif
}

/*******************  FUNCTION  *********************/
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
		EXPECT_EQ(10, site->line);
	#endif
}

/*******************  FUNCTION  *********************/
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

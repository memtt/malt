/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.0-dev
             DATE     : 08/2019
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <SymbolSolver.hpp>
#include <common/Options.hpp>
#include <common/SimpleAllocator.hpp>
#include <common/GTestHelper.hpp>

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
	//EXPECT_EQ("int main(int argc, char ** argv)", std::string(solver.getName((void*)main)));
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

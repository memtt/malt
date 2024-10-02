/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.3
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/portability/tests/TestCompiler.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2022 - 2024
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Compiler.hpp"
#include <common/Options.hpp>
#include <common/SimpleAllocator.hpp>
#include <core/SymbolSolver.hpp>

/**********************************************************/
using namespace MALT;
using namespace testing;

/**********************************************************/
extern "C" {
	int notMangledCFunction(bool p1, int p2)
	{
		return p2;
	}
}

/**********************************************************/
std::string mangledCppFunction(bool p1, int p2)
{
	return "ok";
}

/**********************************************************/
std::string getMangledName(void* addr)
{
	void * callsite = (void*)((size_t)addr + 1);
	SymbolSolver solver;
	solver.registerAddress(callsite);
	solver.solveNames();
	int funcId = solver.getCallSiteInfo(callsite)->function;
	return solver.getString(funcId);
}

/**********************************************************/
TEST(TestCompiler,demangleCppNames_mangled)
{
	std::string mangled = getMangledName((void*)mangledCppFunction);
	std::string demangled = Compiler::demangleCppNames(mangled);
	EXPECT_THAT(demangled, MatchesRegex("^mangledCppFunction(\\[.*\\])\\(bool, int\\)$")) << "mangled=" << mangled;
}

/**********************************************************/
TEST(TestCompiler,demangleCppNames_not_mangled)
{
	std::string mangled = getMangledName((void*)notMangledCFunction);
	std::string demangled = Compiler::demangleCppNames(mangled);
	EXPECT_THAT(demangled, MatchesRegex("^notMangledCFunction()")) << "mangled=" << mangled;
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

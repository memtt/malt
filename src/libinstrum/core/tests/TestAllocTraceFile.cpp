/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 10/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/core/tests/TestAllocTraceFile.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2022
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include <AllocTraceFile.hpp>
#include <common/Options.hpp>
#include <stacks/BacktraceStack.hpp>

/**********************************************************/
using namespace MALT;

/**********************************************************/
TEST(TestAllocTraceFile, constructor)
{
	const char path[] = "./test-teace-file.malt.trace";
	unlink(path);
	AllocTraceFile trace(path);
	EXPECT_EQ(0, unlink(path));
}

/**********************************************************/
/*TEST(TestAllocTraceFile, trace)
{
	//build tracer
	const char path[] = "./test-teace-file.malt.trace";
	unlink(path);
	AllocTraceFile trace(path);

	//build infos
	BacktraceStack stack;
	stack.loadCurrentStack();
	ticks allocTime = Clock::getticks();
	ticks lifeTime = 10000;

	//dump many
	for (int i = 0; i < 1000 ; i++) {
		void * ptr = malloc(100);
		trace.traceChunk(&stack, &stack, ptr, 100, allocTime, lifeTime);
	}

	//flush
	trace.close();

	//check size
	struct stat statres;
	int status = stat(path, &statres);
	EXPECT_EQ(0, status);
	EXPECT_EQ(1000 * sizeof(AllocTracerChunk), statres.st_size);

	//unlink
	EXPECT_EQ(0, unlink(path));
}*/

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

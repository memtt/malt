/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 07/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/tests/TestValgrindOutput.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014 - 2019
*    AUTHOR   : Bastien Levasseur - 2024
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include <common/Options.hpp>
#include <core/ValgrindOutput.hpp>

/**********************************************************/
using namespace MALT;

/**********************************************************/
void test1(void) {}
void test2(void) {}
void test3(void) {}

/**********************************************************/
void *CST_STACK_1[] = {(void*)0x1,(void*)0x2,(void*)0x2,(void*)0x2,(void*)0x2,(void*)0x2,(void*)0x3};
const char CST_VALUE_1[] =  "version: 1\n\
creator: MALT-1.3.1\n\
pid: 1\n\
cmd: TestValgrindOutput\n\
part: 1\n\
\n\
desc: I1 cache: \n\
desc: D1 cache: \n\
desc: LL cache: \n\
\n\
desc: Trigger: Program termination\n\
\n\
positions: line\n\
events: AllocCnt FreeCnt MemOps AllocSum FreeSum Leaks AliveReqMax\n\
\n\
ob=unknown\n\
fi=unknown\n\
fn=[0x1]\n\
0 1 0 1 2 0 2 2\n\
\n\
ob=unknown\n\
fi=unknown\n\
fn=[0x2]\n\
0 0 0 0 0 0 0 0\n\
cob=unknown\n\
cfi=unknown\n\
cfn=[0x1]\n\
0 1 0 1 2 0 2 2\n\
cob=unknown\n\
cfi=unknown\n\
cfn=[0x2]\n\
0 1 0 1 2 0 2 2\n\
\n\
ob=unknown\n\
fi=unknown\n\
fn=[0x3]\n\
0 0 0 0 0 0 0 0\n\
cob=unknown\n\
cfi=unknown\n\
cfn=[0x2]\n\
0 1 0 1 2 0 2 2\n\
\n";

/**********************************************************/
/** Override getpid() to return a fix value. **/
pid_t getpid(void)
{
	return 1;
}

/**********************************************************/
TEST(ValgrindOutput,testRecursive)
{
	ValgrindOutput output;
	Stack stack(CST_STACK_1,7,STACK_ORDER_ASC, MALT::DOMAIN_C);
	SimpleCallStackNode stackNode(stack);
	stackNode.getInfo().onAllocEvent(2,0);
	SymbolSolver symbols;
	symbols.loadProcMap();
	stack.solveSymbols(symbols);
	symbols.solveNames();
	output.pushStackInfo(stackNode,symbols);
	
	std::stringstream buffer;
	output.writeAsCallgrind(buffer,symbols);
	//output.writeAsCallgrind("test.callgrind",symbols);
	
	EXPECT_EQ(CST_VALUE_1,buffer.str());
	
	test1();test2();test3();
}

/**********************************************************/
int main(int argc, char ** argv)
{
	//init internal allocator
	gblInternaAlloc = new SimpleAllocator(true);

	//init options
	initGlobalOptions();
	
	// This allows the user to override the flag on the command line.
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

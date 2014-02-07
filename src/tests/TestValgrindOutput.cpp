/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <core/ValgrindOutput.hpp>

/***************** USING NAMESPACE ******************/
using namespace MATT;

/*******************  FUNCTION  *********************/
void test1(void) {};
void test2(void) {};
void test3(void) {};

/********************** CONSTS **********************/
void *CST_STACK_1[] = {(void*)0x1,(void*)0x2,(void*)0x2,(void*)0x2,(void*)0x2,(void*)0x2,(void*)0x3};
const char CST_VALUE_1[] =  "version: 1\n\
creator: ATT-0.0.0\n\
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
events: AllocCnt FreeCnt MemOps\n\
\n\
ob=unknown\n\
fi=unknown\n\
fn=0x1\n\
0 1 0 1\n\
\n\
ob=unknown\n\
fi=unknown\n\
fn=0x2\n\
0 1 0 1\n\
cob=unknown\n\
cfi=unknown\n\
cfn=0x1\n\
calls=1 0\n\
0 1 0 1\n\
cob=unknown\n\
cfi=unknown\n\
cfn=0x2\n\
calls=1 0\n\
0 1 0 1\n\
\n\
ob=unknown\n\
fi=unknown\n\
fn=0x3\n\
0 1 0 1\n\
cob=unknown\n\
cfi=unknown\n\
cfn=0x2\n\
calls=1 0\n\
0 1 0 1\n\
\n";

/*******************  FUNCTION  *********************/
/** Override getpid() to return a fix value. **/
pid_t getpid(void)
{
	return 1;
}

/*******************  FUNCTION  *********************/
TEST(ValgrindOutput,testRecursive)
{
	ValgrindOutput output;
	Stack stack(CST_STACK_1,7,STACK_ORDER_ASC);
	SimpleCallStackNode stackNode(stack);
	stackNode.getInfo().alloc.count=1;
	stackNode.getInfo().alloc.max=2;
	stackNode.getInfo().alloc.min=2;
	stackNode.getInfo().alloc.sum=2;
	SymbolResolver symbols;
	symbols.loadProcMap();
	stack.resolveSymbols(symbols);
	symbols.resolveNames();
	output.pushStackInfo(stackNode,symbols);
	
	std::stringstream buffer;
	output.writeAsCallgrind(buffer,symbols);
	
	EXPECT_EQ(CST_VALUE_1,buffer.str());
	
	test1();test2();test3();
}

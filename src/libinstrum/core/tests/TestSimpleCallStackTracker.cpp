/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 03/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/core/tests/TestSimpleCallStackTracker.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include <common/SimpleAllocator.hpp>
#include <core/SimpleStackTracker.hpp>
#include <stacks/Stack.hpp>
#include <json/ConvertToJson.h>
#include <portability/OS.hpp>

/**********************************************************/
using namespace MALT;

/**********************************************************/
void functionStackA() {};
void functionStackB() {};
void functionStackC() {};

/**********************************************************/
/*
void solveSymbols(SymbolSolver & symbolResolver);
void fillValgrindOut(MALT::ValgrindOutput& out, MALT::SymbolSolver& symbolResolver) const;
*/
const char * CST_REF_1 = "{\n\
\t\"stats\":[{\n\
\t\t\"stack\":[\"0x20\", \"0x40\"],\n\
\t\t\"infos\":{\n\
\t\t\t\"countZeros\":0,\n\
\t\t\t\"maxAliveReq\":4096,\n\
\t\t\t\"aliveReq\":0,\n\
\t\t\t\"alloc\":{\n\
\t\t\t\t\"count\":3,\n\
\t\t\t\t\"min\":64,\n\
\t\t\t\t\"max\":4096,\n\
\t\t\t\t\"sum\":4288\n\
\t\t\t},\n\
\t\t\t\"free\":{\n\
\t\t\t\t\"count\":3,\n\
\t\t\t\t\"min\":64,\n\
\t\t\t\t\"max\":4096,\n\
\t\t\t\t\"sum\":4288\n\
\t\t\t},\n\
\t\t\t\"mmap\":{\n\
\t\t\t\t\"count\":1,\n\
\t\t\t\t\"min\":4096,\n\
\t\t\t\t\"max\":4096,\n\
\t\t\t\t\"sum\":4096\n\
\t\t\t},\n\
\t\t\t\"munmap\":{\n\
\t\t\t\t\"count\":1,\n\
\t\t\t\t\"min\":4096,\n\
\t\t\t\t\"max\":4096,\n\
\t\t\t\t\"sum\":4096\n\
\t\t\t},\n\
\t\t\t\"lifetime\":{\n\
\t\t\t\t\"count\":2,\n\
\t\t\t\t\"min\":50,\n\
\t\t\t\t\"max\":100,\n\
\t\t\t\t\"sum\":150\n\
\t\t\t},\n\
\t\t\t\"globalPeak\":0,\n\
\t\t\t\"reallocCount\":0,\n\
\t\t\t\"reallocSumDelta\":0\n\
\t\t}\n\
\t}],\n\
\t\"count\":1\n\
}\
";

const char * CST_REF_2 = "{\n\
\t\"map\":[],\n\
\t\"strings\":[\"??\", \"TestSimpleCallStackTracker.cpp\", \"functionStackA\", \"TestSimpleStackTracker\", \"functionStackB\", \"functionStackC\"],\n\
\t\"instr\":{\n\
\t\t\"0x10\":{\n\
\t\t\t\"file\":1,\n\
\t\t\t\"function\":2,\n\
\t\t\t\"line\":23,\n\
\t\t\t\"binary\":3\n\
\t\t},\n\
\t\t\"0x20\":{\n\
\t\t\t\"file\":1,\n\
\t\t\t\"function\":4,\n\
\t\t\t\"line\":24,\n\
\t\t\t\"binary\":3\n\
\t\t},\n\
\t\t\"0x30\":{\n\
\t\t\t\"file\":1,\n\
\t\t\t\"function\":5,\n\
\t\t\t\"line\":25,\n\
\t\t\t\"binary\":3\n\
\t\t}\n\
\t}\n\
}\
";

const char * CST_REF_3 = "\
version: 1\n\
creator: MALT-1.3.1\n\
pid: 10000\n\
cmd: TestSimpleCallStackTracker\n\
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
fl=TestSimpleCallStackTracker.cpp\n\
fn=functionStackA\n\
23 0 0 0 0 0 0 0\n\
\n\
ob=unknown\n\
fl=TestSimpleCallStackTracker.cpp\n\
fn=functionStackB\n\
24 0 0 0 0 0 0 0\n\
cob=unknown\n\
cfl=TestSimpleCallStackTracker.cpp\n\
cfn=functionStackA\n\
24 0 0 0 0 0 0 0\n\
\n\
ob=unknown\n\
fl=TestSimpleCallStackTracker.cpp\n\
fn=functionStackC\n\
25 0 0 0 0 0 0 0\n\
cob=unknown\n\
cfl=TestSimpleCallStackTracker.cpp\n\
cfn=functionStackB\n\
25 0 0 0 0 0 0 0\n\
\n\
";

/**********************************************************/
unsigned int MALT::OSUnix::getPID(void)
{
	return 10000;
}

/**********************************************************/
//for pre init of global allocator
class ForceInit
{
	public:
		ForceInit(void) {gblInternaAlloc = new SimpleAllocator(true);};
};
ForceInit gblForceInit;

/**********************************************************/
TEST(TestSimpleStackTracker, constructor)
{
	SimpleStackTracker tracker;
}

/**********************************************************/
TEST(TestSimpleStackTracker, getBacktraceInfo)
{
	//build a stack
	void * cstStackFrames[] {(void*)0x10, (void*)0x20, (void*)0x40};
	Stack stack(cstStackFrames, 3, STACK_ORDER_ASC, DOMAIN_C);
	Stack stack2(cstStackFrames + 1, 2, STACK_ORDER_ASC, DOMAIN_C);
	
	//build tracker
	SimpleStackTracker tracker;

	//load stack in
	SimpleCallStackNode & node = tracker.getBacktraceInfo(stack, 1);
	
	//checks
	ASSERT_EQ(node.getCallStack(), stack2);

	//make an allocatoin
	auto & infos = node.getInfo();
	infos.onAllocEvent(128, 1);

	//search again
	ASSERT_EQ(&tracker.getBacktraceInfo(stack, 1).getInfo(), &infos);
}

/**********************************************************/
TEST(TestSimpleStackTracker, outputStream)
{
	//build a stack
	void * cstStackFrames[] {(void*)0x10, (void*)0x20, (void*)0x40};
	Stack stack(cstStackFrames, 3, STACK_ORDER_ASC, DOMAIN_C);
	
	//build tracker
	SimpleStackTracker tracker;

	//load stack in
	tracker.getBacktraceInfo(stack, 1).getInfo().onAllocEvent(128, 1);

	//to stream
	std::stringstream out;
	out << tracker;

	//check
	ASSERT_EQ(out.str(), "[ 0x20 0x40  ] count = 0\n");
}

/**********************************************************/
TEST(TestSimpleStackTracker, convertToJson)
{
	//build a stack
	void * cstStackFrames[] {(void*)0x10, (void*)0x20, (void*)0x40};
	Stack stack(cstStackFrames, 3, STACK_ORDER_ASC, DOMAIN_C);
	
	//build tracker
	SimpleStackTracker tracker;

	//load stack in
	auto & infos = tracker.getBacktraceInfo(stack, 1).getInfo();
	infos.onAllocEvent(64, 1);
	infos.onFreeEvent(64, 1);
	infos.onFreeLinkedMemory(64, 50, 1);
	infos.onAllocEvent(128, 2);
	infos.onFreeEvent(128, 2);
	infos.onFreeLinkedMemory(128, 100, 2);
	infos.onMmap(4096, 3);
	infos.onMunmap(4096, 3, false);

	//to stream
	std::stringstream out;
	htopml::convertToJson(out, tracker);

	//check
	ASSERT_EQ(out.str(), CST_REF_1);
}

/**********************************************************/
TEST(TestSimpleStackTracker, solveSymbols)
{
	//build a stack
	void * cstStackFrames[] {(void*)0x10, (void*)0x20, (void*)0x30};
	Stack stack(cstStackFrames, 3, STACK_ORDER_ASC, DOMAIN_C);
	
	//build tracker
	SimpleStackTracker tracker;

	//load stack in
	auto & infos = tracker.getBacktraceInfo(stack, 0);

	//resolve
	SymbolSolver solver;
	solver.registerFunctionSymbol((void*)0x10, "TestSimpleStackTracker", "functionStackA", "TestSimpleCallStackTracker.cpp", 23);
	solver.registerFunctionSymbol((void*)0x20, "TestSimpleStackTracker", "functionStackB", "TestSimpleCallStackTracker.cpp", 24);
	solver.registerFunctionSymbol((void*)0x30, "TestSimpleStackTracker", "functionStackC", "TestSimpleCallStackTracker.cpp", 25);

	//port in tracker
	tracker.solveSymbols(solver);

	//get one of them
	std::stringstream out;
	htopml::convertToJson(out, solver);

	//compare
	//ASSERT_THAT(value2, MatchesRegex(CST_REF_2));
	ASSERT_EQ(CST_REF_2, out.str());
}

TEST(TestSimpleStackTracker, ValgrindOutput)
{
	//build a stack
	void * cstStackFrames[] {(void*)0x10, (void*)0x20, (void*)0x30};
	Stack stack(cstStackFrames, 3, STACK_ORDER_ASC, DOMAIN_C);
	
	//build tracker
	SimpleStackTracker tracker;

	//load stack in
	auto & infos = tracker.getBacktraceInfo(stack, 0);

	//resolve
	SymbolSolver solver;
	solver.registerFunctionSymbol((void*)0x10, "TestSimpleStackTracker", "functionStackA", "TestSimpleCallStackTracker.cpp", 23);
	solver.registerFunctionSymbol((void*)0x20, "TestSimpleStackTracker", "functionStackB", "TestSimpleCallStackTracker.cpp", 24);
	solver.registerFunctionSymbol((void*)0x30, "TestSimpleStackTracker", "functionStackC", "TestSimpleCallStackTracker.cpp", 25);

	//port in tracker
	ValgrindOutput valgrind;
	tracker.fillValgrindOut(valgrind, solver);

	//get one of them
	std::stringstream out;
	valgrind.writeAsCallgrind(out, solver);

	//compare
	//ASSERT_THAT(value2, MatchesRegex(CST_REF_2));
	ASSERT_EQ(out.str(), CST_REF_3);
}

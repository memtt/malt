/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 09/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/core/tests/TestSimpleCallStackNode.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include <common/SimpleAllocator.hpp>
#include <core/SimpleCallStackNode.hpp>
#include <stacks/Stack.hpp>
#include <json/ConvertToJson.h>

/**********************************************************/
using namespace MALT;

/**********************************************************/
const char * CST_RESULT_1 = "{\n\t\
\"stack\":[\"0x10\", \"0x20\", \"0x40\"],\n\
\t\"infos\":{\n\
\t\t\"countZeros\":0,\n\
\t\t\"maxAliveReq\":0,\n\
\t\t\"aliveReq\":0,\n\
\t\t\"maxAliveReqGPU\":0,\n\
\t\t\"aliveReqGPU\":0,\n\
\t\t\"alloc\":{\n\
\t\t\t\"count\":0,\n\
\t\t\t\"min\":0,\n\
\t\t\t\"max\":0,\n\
\t\t\t\"sum\":0\n\
\t\t},\n\t\t\"free\":{\n\
\t\t\t\"count\":0,\n\
\t\t\t\"min\":0,\n\
\t\t\t\"max\":0,\n\
\t\t\t\"sum\":0\n\
\t\t},\n\
\t\t\"gpuAlloc\":{\n\
\t\t\t\"count\":0,\n\
\t\t\t\"min\":0,\n\
\t\t\t\"max\":0,\n\
\t\t\t\"sum\":0\n\
\t\t},\n\t\t\"gpuFree\":{\n\
\t\t\t\"count\":0,\n\
\t\t\t\"min\":0,\n\
\t\t\t\"max\":0,\n\
\t\t\t\"sum\":0\n\
\t\t},\n\
\t\t\"mmap\":{\n\
\t\t\t\"count\":0,\n\
\t\t\t\"min\":0,\n\
\t\t\t\"max\":0,\n\
\t\t\t\"sum\":0\n\
\t\t},\n\t\t\"munmap\":{\n\
\t\t\t\"count\":0,\n\
\t\t\t\"min\":0,\n\
\t\t\t\"max\":0,\n\
\t\t\t\"sum\":0\n\
\t\t},\n\
\t\t\"lifetime\":{\n\
\t\t\t\"count\":0,\n\
\t\t\t\"min\":0,\n\
\t\t\t\"max\":0,\n\
\t\t\t\"sum\":0\n\
\t\t},\n\
\t\t\"globalPeak\":0,\n\
\t\t\"globalPeakGPU\":0,\n\
\t\t\"reallocCount\":0,\n\
\t\t\"reallocSumDelta\":0\n\
\t}\n\
}";

/**********************************************************/
const char * CST_RESULT_2 = "{\n\t\
\"stack\":[\"0x20\", \"0x40\"],\n\
\t\"infos\":{\n\
\t\t\"countZeros\":0,\n\
\t\t\"maxAliveReq\":0,\n\
\t\t\"aliveReq\":0,\n\
\t\t\"maxAliveReqGPU\":0,\n\
\t\t\"aliveReqGPU\":0,\n\
\t\t\"alloc\":{\n\
\t\t\t\"count\":0,\n\
\t\t\t\"min\":0,\n\
\t\t\t\"max\":0,\n\
\t\t\t\"sum\":0\n\
\t\t},\n\t\t\"free\":{\n\
\t\t\t\"count\":0,\n\
\t\t\t\"min\":0,\n\
\t\t\t\"max\":0,\n\
\t\t\t\"sum\":0\n\
\t\t},\n\
\t\t\"gpuAlloc\":{\n\
\t\t\t\"count\":0,\n\
\t\t\t\"min\":0,\n\
\t\t\t\"max\":0,\n\
\t\t\t\"sum\":0\n\
\t\t},\n\t\t\"gpuFree\":{\n\
\t\t\t\"count\":0,\n\
\t\t\t\"min\":0,\n\
\t\t\t\"max\":0,\n\
\t\t\t\"sum\":0\n\
\t\t},\n\
\t\t\"mmap\":{\n\
\t\t\t\"count\":0,\n\
\t\t\t\"min\":0,\n\
\t\t\t\"max\":0,\n\
\t\t\t\"sum\":0\n\
\t\t},\n\t\t\"munmap\":{\n\
\t\t\t\"count\":0,\n\
\t\t\t\"min\":0,\n\
\t\t\t\"max\":0,\n\
\t\t\t\"sum\":0\n\
\t\t},\n\
\t\t\"lifetime\":{\n\
\t\t\t\"count\":0,\n\
\t\t\t\"min\":0,\n\
\t\t\t\"max\":0,\n\
\t\t\t\"sum\":0\n\
\t\t},\n\
\t\t\"globalPeak\":0,\n\
\t\t\"globalPeakGPU\":0,\n\
\t\t\"reallocCount\":0,\n\
\t\t\"reallocSumDelta\":0\n\
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
TEST(TestSimpleCallStackNode,testConstructor)
{
	//build a stack
	void * cstStackFrames[] {(void*)0x10, (void*)0x20, (void*)0x40};
	Stack stack(cstStackFrames, 3, STACK_ORDER_ASC, DOMAIN_C);
	
	//load it in
	SimpleCallStackNode node(stack, 0);
}

/**********************************************************/
TEST(TestSimpleCallStackNode,convertToJson_1)
{
	//build a stack
	void * cstStackFrames[] {(void*)0x10, (void*)0x20, (void*)0x40};
	Stack stack(cstStackFrames, 3, STACK_ORDER_ASC, DOMAIN_C);
	
	//load it in
	SimpleCallStackNode node(stack, 0);

	//convert
	std::stringstream out;
	htopml::convertToJson(out, node);

	//compare
	EXPECT_EQ(out.str(), CST_RESULT_1);
}

/**********************************************************/
TEST(TestSimpleCallStackNode,convertToJson_2)
{
	//build a stack
	void * cstStackFrames[] {(void*)0x10, (void*)0x20, (void*)0x40};
	Stack stack(cstStackFrames, 3, STACK_ORDER_ASC, DOMAIN_C);
	
	//load it in
	SimpleCallStackNode node(stack, 1);

	//convert
	std::stringstream out;
	htopml::convertToJson(out, node);

	//compare
	EXPECT_EQ(out.str(), CST_RESULT_2);
}

/**********************************************************/
TEST(TestSimpleCallStackNode,outputStream)
{
	//build a stack
	void * cstStackFrames[] {(void*)0x10, (void*)0x20, (void*)0x40};
	Stack stack(cstStackFrames, 3, STACK_ORDER_ASC, DOMAIN_C);
	
	//load it in
	SimpleCallStackNode node(stack, 1);

	//convert
	std::stringstream out;
	out << node;
 
	//compare
	EXPECT_EQ(out.str(), "[ 0x20 0x40  ] count = 0");
}

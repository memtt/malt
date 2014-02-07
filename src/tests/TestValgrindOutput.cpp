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
	
	EXPECT_EQ("",buffer.str());
	
	test1();test2();test3();
}

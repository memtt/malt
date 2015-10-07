/*****************************************************
             PROJECT  : MALT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <EnterExitStack.hpp>
#include <common/SimpleAllocator.hpp>

/***************** USING NAMESPACE ******************/
using namespace MALTV2;

/*******************  FUNCTION  *********************/
TEST(Stack,test1)
{
	EnterExitStack stack;
	
	for (int i = 0 ; i < 6 ; i++)
		stack.enterFunction((void*)(size_t)i);
	
	stack.exitFunction((void*)0x5);
	
	for (int i = 0 ; i < 5 ; i++)
		EXPECT_EQ((void*)(size_t)(4-i),stack[i]);
}


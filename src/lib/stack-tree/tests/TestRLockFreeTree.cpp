/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <json/ConvertToJson.h>
#include <common/SimpleAllocator.hpp>
#include <stack-tree/RLockFreeTree.hpp>

/***************** USING NAMESPACE ******************/
using namespace MALT;

void * CST_VALUE_1[] = {(void*)0x1,(void*)0x2,(void*)0x3,(void*)0x4};
const char CST_VALUE_2[] = "{\n\
	\"stats\":[{\n\
		\"stack\":[\"0xccc\", \"0xbbb\"],\n\
		\"infos\":11\n\
	}, {\n\
		\"stack\":[\"0xaaa\"],\n\
		\"infos\":10\n\
	}],\n\
	\"count\":2\n\
}";


/*******************  FUNCTION  *********************/
TEST(RLockFreeTree,constructor)
{
	RLockFreeTree<int> tree;
}

/*******************  FUNCTION  *********************/
TEST(RLockFreeTree,getRoot)
{
	RLockFreeTree<int> tree;
	RLockFreeTree<int>::Handler handler1 = tree.getRoot();
	RLockFreeTree<int>::Handler handler2 = tree.getRoot();
	
	EXPECT_EQ(handler1,handler2);
	EXPECT_NE((void*)NULL,handler1);
}

/*******************  FUNCTION  *********************/
TEST(RLockFreeTree,getChild)
{
	RLockFreeTree<int> tree;
	RLockFreeTree<int>::Handler handler1 = tree.getRoot();
	RLockFreeTree<int>::Handler handler2 = tree.getChild(handler1,(void*)0xAAA);
	RLockFreeTree<int>::Handler handler3 = tree.getChild(handler1,(void*)0xAAA);
	RLockFreeTree<int>::Handler handler4 = tree.getChild(handler1,(void*)0xBBB);
	
	EXPECT_EQ(handler2,handler3);
	EXPECT_NE(handler1,handler2);
	EXPECT_NE(handler1,handler4);
}

/*******************  FUNCTION  *********************/
TEST(RLockFreeTree,getData)
{
	RLockFreeTree<int> tree;
	RLockFreeTree<int>::Handler handler1 = tree.getRoot();
	RLockFreeTree<int>::Handler handler2 = tree.getChild(handler1,(void*)0xAAA);
	RLockFreeTree<int>::Handler handler3 = tree.getChild(handler1,(void*)0xBBB);
	
	int * data21 = tree.getData(handler2);
	int * data22 = tree.getData(handler2);
	int * data31 = tree.getData(handler3);
	int * data32 = tree.getData(handler3);
	
	EXPECT_EQ(data21,data22);
	EXPECT_EQ(data31,data32);
	EXPECT_NE(data21,data31);
}

/*******************  FUNCTION  *********************/
TEST(RLockFreeTree,toJson)
{
	RLockFreeTree<int> tree;
	RLockFreeTree<int>::Handler handler1 = tree.getRoot();
	RLockFreeTree<int>::Handler handler2 = tree.getChild(handler1,(void*)0xAAA);
	RLockFreeTree<int>::Handler handler3 = tree.getChild(handler1,(void*)0xBBB);
	RLockFreeTree<int>::Handler handler4 = tree.getChild(handler3,(void*)0xCCC);
	
	*tree.getData(handler2) = 10;
	*tree.getData(handler4) = 11;
	
	std::stringstream out;
	htopml::convertToJson(out,tree);
	
	EXPECT_EQ(CST_VALUE_2,out.str());
}

/*******************  FUNCTION  *********************/
TEST(RLockFreeTree,parallelUse)
{
	RLockFreeTree<int> tree;
	#pragma omp parallel
	{
		for (int j = 0 ; j < 1000 ; j++)
		{
			RLockFreeTree<int>::Handler handler = tree.getRoot();
			
			for (int i = 0 ; i < 10 ; i++)
				handler = tree.getChild(handler,(void*)(size_t)(i+1));
		}
	}
}

/*******************  FUNCTION  *********************/
int main(int argc, char ** argv)
{
	//init internal allocator
	gblInternaAlloc = new SimpleAllocator(true);
	
	// This allows the user to override the flag on the command line.
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/common/tests/TestArray.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include "Array.hpp"
#include <json/ConvertToJson.h>

/**********************************************************/
using namespace MALT;

/**********************************************************/
TEST(Array,constructor)
{
	//check allocation of default size
	Array<int> array(10,64,true);
	EXPECT_EQ(0,array.getSize());
	EXPECT_EQ(10,array.getBufferSize());
}

/**********************************************************/
TEST(Array,push_back)
{
	//prepare array
	Array<int> array(10,64,true);
	
	//fill
	for (int i = 0 ; i < 20 ; i++)
		array.push_back(i);
	
	//check current size
	EXPECT_EQ(20,array.getSize());
	EXPECT_EQ(20,array.getBufferSize());
	
	//check content and operator []
	for (int i = 0 ; i < 20 ; i++)
		ASSERT_EQ(array[i],i);
}

/**********************************************************/
TEST(Array,getBuffer)
{
	//prepare array
	Array<int> array(10,64,true);
	
	//check
	const int * buf1 = array.getBuffer();
	const int * buf2 = array.getBuffer();
	
	//check
	EXPECT_EQ(buf1,buf2);
}

/**********************************************************/
TEST(Array,constOperatorArray)
{
	//prepare array
	Array<int> array(10,64,true);
	
	//fill
	for (int i = 0 ; i < 20 ; i++)
		array.push_back(i);
	
	//check current size
	EXPECT_EQ(20,array.getSize());
	EXPECT_EQ(20,array.getBufferSize());
	
	const Array<int> & arrayCst = array;
	
	//check content and operator []
	for (int i = 0 ; i < 20 ; i++)
		ASSERT_EQ(arrayCst[i],i);
}

/**********************************************************/
TEST(Array,grow)
{
	//prepare array
	Array<int> array(4,16,true);
	EXPECT_EQ(4,array.getBufferSize());
	
	//fill 4
	int i = 0;
	for (; i < 4 ; i++) array.push_back(i);
	EXPECT_EQ(4,array.getBufferSize());
	
	for (; i < 8 ; i++) array.push_back(i);
	EXPECT_EQ(8,array.getBufferSize());
	
	for (; i < 16 ; i++) array.push_back(i);
	EXPECT_EQ(16,array.getBufferSize());

	for (; i < 32 ; i++) array.push_back(i);
	EXPECT_EQ(32,array.getBufferSize());

	// now grow linearly
	for (; i < 48 ; i++) array.push_back(i);
	EXPECT_EQ(48,array.getBufferSize());
	
	for (; i < 64 ; i++) array.push_back(i);
	EXPECT_EQ(64,array.getBufferSize());
}

/**********************************************************/
TEST(Array,pop)
{
	//prepare array
	Array<int> array(4,16,true);
	EXPECT_EQ(4,array.getBufferSize());

	for (int i = 0 ; i < 4 ; i++) array.push_back(i);
	EXPECT_EQ(4,array.getSize());
	
	array.pop();
	EXPECT_EQ(3,array.getSize());
}

/**********************************************************/
TEST(Array,set)
{
	//prepare array
	Array<int> array(4,16,true);
	for (int i = 0 ; i < 4 ; i++) array.push_back(i);
	
	//copy
	Array<int> array2(4,16,true);
	array2.set(array);
	
	//check
	EXPECT_EQ(4,array.getSize());
	for (int i = 0 ; i < 4 ; i++) 
		EXPECT_EQ(array[i],array2[i]);
}

/**********************************************************/
TEST(Array,operatorEqual)
{
	//prepare array
	Array<int> array(4,16,true);
	for (int i = 0 ; i < 4 ; i++) array.push_back(i);
	
	//copy
	Array<int> array2(4,16,true);
	array2 = array;
	
	//check
	EXPECT_EQ(4,array.getSize());
	for (int i = 0 ; i < 4 ; i++) 
		EXPECT_EQ(array[i],array2[i]);
}

/**********************************************************/
TEST(Array,convertToJson)
{
	//prepare array
	Array<int> array(4,16,true);
	for (int i = 0 ; i < 4 ; i++) array.push_back(i);
	
	//convert
	std::stringstream out;
	htopml::convertToJson(out,array);
	
	//check
	EXPECT_EQ("[0, 1, 2, 3]",out.str());
}

/**********************************************************/
int main(int argc, char ** argv)
{
	//init internal allocator
	gblInternaAlloc = new SimpleAllocator(true);
	
	// This allows the user to override the flag on the command line.
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

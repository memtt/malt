/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <common/Array.hpp>

/***************** USING NAMESPACE ******************/
using namespace MATT;

/*******************  FUNCTION  *********************/
TEST(Array,constructor)
{
	Array<int> array(10,64,true);
	EXPECT_EQ(0,array.getSize());
	EXPECT_EQ(10,array.getBufferSize());
}

/*******************  FUNCTION  *********************/
TEST(Array,push_back)
{
	Array<int> array(10,64,true);
	
	for (int i = 0 ; i < 20 ; i++)
		array.push_back(i);
	
	EXPECT_EQ(20,array.getSize());
	EXPECT_EQ(20,array.getBufferSize());
	
	for (int i = 0 ; i < 20 ; i++)
		ASSERT_EQ(array[i],i);
}

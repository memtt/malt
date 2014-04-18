/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include "Array.hpp"

/***************** USING NAMESPACE ******************/
using namespace MATT;

/*******************  FUNCTION  *********************/
TEST(Array,constructor)
{
	//check allocation of default size
	Array<int> array(10,64,true);
	EXPECT_EQ(0,array.getSize());
	EXPECT_EQ(10,array.getBufferSize());
}

/*******************  FUNCTION  *********************/
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

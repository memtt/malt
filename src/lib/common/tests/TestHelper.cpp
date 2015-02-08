/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <common/Helpers.hpp>
#include <sstream>

/***************** USING NAMESPACE ******************/
using namespace MATT;

/*******************  FUNCTION  *********************/
TEST(Array,printValue)
{
	std::stringstream out1;
	Helpers::printValue(out1,16,"B");
	EXPECT_EQ("16B",out1.str());
	
	std::stringstream out2;
	Helpers::printValue(out2,4096,"B");
	EXPECT_EQ("4KB",out2.str());
}

/*******************  FUNCTION  *********************/
TEST(Array,getFileId)
{
	Helpers::getFileId();
}

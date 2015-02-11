/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
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
	EXPECT_EQ(" 16.0  B",out1.str());
	
	std::stringstream out2;
	Helpers::printValue(out2,4096,"B");
	EXPECT_EQ("  4.0 KB",out2.str());
}

/*******************  FUNCTION  *********************/
TEST(Array,getFileId)
{
	Helpers::getFileId();
}

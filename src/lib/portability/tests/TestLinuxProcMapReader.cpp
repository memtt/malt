/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <LinuxProcMapReader.hpp>

/***************** USING NAMESPACE ******************/
using namespace MATT;

/*******************  FUNCTION  *********************/
void testFunction(void)
{
}

/*******************  FUNCTION  *********************/
TEST(LinuxProcMapReader,constructor)
{
	LinuxProcMapReader reader;
}

/*******************  FUNCTION  *********************/
TEST(LinuxProcMapReader,load)
{
	LinuxProcMapReader reader;
	reader.load();
}

/*******************  FUNCTION  *********************/
TEST(LinuxProcMapReader,getEntry)
{
	LinuxProcMapReader reader;
	reader.load();
	EXPECT_NE((void*)NULL,reader.getEntry((void*)testFunction));
}

/*******************  FUNCTION  *********************/
TEST(LinuxProcMapReader,iterator)
{
	LinuxProcMapReader reader;
	
	std::stringstream out;
	for (LinuxProcMapReader::const_iterator it = reader.begin() ; it != reader.end() ; ++it)
	{
		out << it->file << std::endl;
	}
}

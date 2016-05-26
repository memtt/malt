/*****************************************************
             PROJECT  : MALT
             VERSION  : 0.2.0
             DATE     : 04/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <tools/ProcPageMapReader.hpp>

/***************** USING NAMESPACE ******************/
using namespace MATT;

/*******************  FUNCTION  *********************/
TEST(ProcPagemapReader,structSize)
{
	EXPECT_EQ(8,sizeof(ProcPageMapEntry));
}

/*******************  FUNCTION  *********************/
TEST(ProcPagemapReader,small)
{
	char buffer[1024];
	size_t phys = ProcPageMapReader::getPhysicalSize(buffer,sizeof(buffer));
	EXPECT_EQ(1024,phys);
}

/*******************  FUNCTION  *********************/
TEST(ProcPagemapReader,medium)
{
	char buffer[4096+1024];
	memset(buffer,0,sizeof(buffer));
	size_t phys = ProcPageMapReader::getPhysicalSize(buffer,sizeof(buffer));
	EXPECT_EQ(4096+1024,phys);
}

/*******************  FUNCTION  *********************/
TEST(ProcPagemapReader,largeFull)
{
	const size_t size = 32*1024*1024;
	char * buffer = new char[size];
	memset(buffer,0,size);
	size_t phys = ProcPageMapReader::getPhysicalSize(buffer,size);
	EXPECT_EQ(size,phys);
}

/*******************  FUNCTION  *********************/
TEST(ProcPagemapReader,largeHalfFull)
{
	const size_t size = 32*1024*1024;
	char * buffer = new char[size];
	memset(buffer,0,size/2);
	size_t phys = ProcPageMapReader::getPhysicalSize(buffer,size);
	EXPECT_LT(size/2,phys);
	if (ProcPageMapReader::hasProcPagemap())
		EXPECT_GT(size,phys);
}

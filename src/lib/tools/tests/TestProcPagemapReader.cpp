/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <tools/ProcPagemapReader.hpp>
#include <common/SimpleAllocator.hpp>

/***************** USING NAMESPACE ******************/
using namespace MALT;

/*******************  FUNCTION  *********************/
TEST(ProcPagemapReader,structSize)
{
	EXPECT_EQ(8ul,sizeof(ProcPageMapEntry));
}

/*******************  FUNCTION  *********************/
TEST(ProcPagemapReader,small)
{
	char buffer[1024];
	size_t phys = ProcPageMapReader::getPhysicalSize(buffer,sizeof(buffer));
	EXPECT_EQ(1024lu,phys);
}

/*******************  FUNCTION  *********************/
TEST(ProcPagemapReader,medium)
{
	char buffer[4096+1024];
	memset(buffer,0,sizeof(buffer));
	size_t phys = ProcPageMapReader::getPhysicalSize(buffer,sizeof(buffer));
	EXPECT_EQ(4096ul+1024ul,phys);
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
	if (ProcPageMapReader::hasProcPagemap()) {
		EXPECT_GT(size,phys);
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

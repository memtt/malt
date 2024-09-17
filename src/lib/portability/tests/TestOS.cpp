/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.2
*    DATE     : 06/2023
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/portability/tests/TestOS.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2019
*    AUTHOR   : Sébastien Valat (INRIA) - 2023
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <OS.hpp>

/**********************************************************/
using namespace MALT;
using namespace testing;

/**********************************************************/
TEST(TestOS,getSignalName)
{
	EXPECT_EQ("SIGUSR1", OS::getSignalName(SIGUSR1));
	EXPECT_EQ("SIGUSR2", OS::getSignalName(SIGUSR2));
	EXPECT_EQ("SIGTERM", OS::getSignalName(SIGTERM));
	EXPECT_EQ("UNKNOWN", OS::getSignalName(-1));
}

/**********************************************************/
TEST(TestOS,getSignalFromName)
{
	EXPECT_EQ(SIGUSR1, OS::getSignalFromName("SIGUSR1"));
	EXPECT_EQ(SIGUSR2, OS::getSignalFromName("SIGUSR2"));
	EXPECT_EQ(SIGTERM, OS::getSignalFromName("SIGTERM"));
}

/**********************************************************/
TEST(TestOS,getProcMemUsage)
{
	//increase the mem usage
	const size_t size = 100*1024*1024;
	void * ptr = OS::mmap(size, false);

	//call
	OSProcMemUsage mem1 = OS::getProcMemoryUsage();
	EXPECT_GE(mem1.virtualMemory, size);
	EXPECT_LE(mem1.physicalMemory, size);

	//touch
	memset(ptr, 0, size);
	OSProcMemUsage mem2 = OS::getProcMemoryUsage();
	EXPECT_GE(mem2.physicalMemory, size);

	//ok
	OS::munmap(ptr, size);
}

/**********************************************************/
TEST(TestOK,getMemoryUsage)
{
	OSMemUsage mem = OS::getMemoryUsage();
	EXPECT_NE(0, mem.totalMemory);
	EXPECT_NE(0, mem.freeMemory);
	EXPECT_NE(0, mem.cached);
	EXPECT_NE(0, mem.directMap4K);
	EXPECT_NE(0, mem.directMap2M);
}

/**********************************************************/
TEST(TestOK,getExeName)
{
	EXPECT_EQ("TestOS", OS::getExeName());
}

/**********************************************************/
TEST(TestOK,getCmdLine)
{
	EXPECT_STREQ("TestOS", basename(OS::getCmdLine().c_str()));
}

/**********************************************************/
TEST(TestOK,getSignalName)
{
	EXPECT_EQ("SIGINT", OS::getSignalName(SIGINT));
	EXPECT_EQ("SIGKILL", OS::getSignalName(SIGKILL));
	EXPECT_EQ("SIGSEGV", OS::getSignalName(SIGSEGV));
	EXPECT_EQ("SIGUSR1", OS::getSignalName(SIGUSR1));
	EXPECT_EQ("SIGUSR2", OS::getSignalName(SIGUSR2));
}

/**********************************************************/
TEST(TestOK,getSignalFromName)
{
	EXPECT_EQ(SIGINT, OS::getSignalFromName("SIGINT"));
	EXPECT_EQ(SIGKILL, OS::getSignalFromName("SIGKILL"));
	EXPECT_EQ(SIGSEGV, OS::getSignalFromName("SIGSEGV"));
	EXPECT_EQ(SIGUSR1, OS::getSignalFromName("SIGUSR1"));
	EXPECT_EQ(SIGUSR2, OS::getSignalFromName("SIGUSR2"));
}

/**********************************************************/
TEST(TestOK,printAvailSigs)
{
	std::stringstream out;
	OS::printAvailSigs(out);
	EXPECT_THAT(out.str(), MatchesRegex("MALT: supported signal: SIGUSR1 \\([0-9]+\\).*"));
	EXPECT_THAT(out.str(), MatchesRegex(".*MALT: supported signal: SIGUSR2 \\([0-9]+\\).*"));
}

/**********************************************************/
static int gblGotSig = -1;
void fakeHandler(int s)
{
	gblGotSig = s;
};

/**********************************************************/
TEST(TestOK,setSigHandler_string)
{
	OS::setSigHandler(fakeHandler, "SIGUSR1");
	gblGotSig = -1;
	kill((pid_t)OS::getPID(), SIGUSR1);
	EXPECT_EQ(gblGotSig, SIGUSR1);
}

/**********************************************************/
TEST(TestOK,setSigHandler_int)
{
	OS::setSigHandler(fakeHandler, SIGUSR2);
	gblGotSig = -1;
	kill((pid_t)OS::getPID(), SIGUSR2);
	EXPECT_EQ(gblGotSig, SIGUSR2);
}

/**********************************************************/
TEST(TestOK,loadTextFile)
{
	std::string content = OS::loadTextFile(CURRENT_SOURCE_DIR "/data.txt");
	EXPECT_EQ(content, "Hello World\nThis is a new line\n"); 
}

/**********************************************************/
TEST(TestOK,mmap)
{
	const size_t size = 1024*1024;
	void * ptr = OS::mmap(size, false);
	ASSERT_NE(nullptr, ptr);
	memset(ptr, 0, size);
	OS::munmap(ptr, size);
}

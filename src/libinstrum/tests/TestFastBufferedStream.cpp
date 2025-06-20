/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 10/2014
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/tests/TestFastBufferedStream.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include <json/FastBufferdStream.hpp>

/**********************************************************/
using namespace htopml;
using namespace std;

/**********************************************************/
class TestFastBufferdStream : public testing::Test
{
	public:
		void SetUp(void);
		void TearDown(void);
	protected:
		stringstream * out;
		FastBufferedStream * bufferd;
};

/**********************************************************/
ostream & operator << (ostream & out, const TestFastBufferdStream & value)
{
	out << "TestFastBufferdStream";
	return out;
}

/**********************************************************/
void TestFastBufferdStream::SetUp(void)
{
	out = new stringstream;
	bufferd = new FastBufferedStream(out,512);
}

/**********************************************************/
void TestFastBufferdStream::TearDown(void)
{
	delete bufferd;
	delete out;
}

/**********************************************************/
TEST_F(TestFastBufferdStream,constructor)
{
}

/**********************************************************/
TEST_F(TestFastBufferdStream,emptyFlush)
{
	bufferd->flush();
}

/**********************************************************/
TEST_F(TestFastBufferdStream,putChar)
{
	EXPECT_EQ("",out->str());
	*bufferd << 'c';
	EXPECT_EQ("",out->str());
	bufferd->flush();
	EXPECT_EQ("c",out->str());
}

/**********************************************************/
TEST_F(TestFastBufferdStream,putCStringShort)
{
	EXPECT_EQ("",out->str());
	*bufferd << "hello";
	EXPECT_EQ("",out->str());
	bufferd->flush();
	EXPECT_EQ("hello",out->str());
}

/**********************************************************/
TEST_F(TestFastBufferdStream,putCStringLong)
{
	//setup long chain
	char buffer[1024];
	char buffer2[1024];
	for (size_t i = 0 ; i < sizeof(buffer) ; i++)
	{
		buffer[i] = 'a';
		buffer2[i] = 'a';
	}
	buffer[sizeof(buffer) - 1] = '\0';
	buffer2[512] = '\0';
	
	//check
	EXPECT_EQ("",out->str());
	*bufferd << buffer;
	EXPECT_EQ(buffer2,out->str());
	bufferd->flush();
	EXPECT_EQ(buffer,out->str());
}

/**********************************************************/
TEST_F(TestFastBufferdStream,putStringShort)
{
	EXPECT_EQ("",out->str());
	*bufferd << string("hello");
	EXPECT_EQ("",out->str());
	bufferd->flush();
	EXPECT_EQ("hello",out->str());
}

/**********************************************************/
TEST_F(TestFastBufferdStream,putStringLong)
{
	string value;
	for (int i = 0 ; i < 1024 ; i++)
		value += 'a';
	
	EXPECT_EQ("",out->str());
	*bufferd << value;
	EXPECT_EQ(value,out->str());
	bufferd->flush();
	EXPECT_EQ(value,out->str());
}

/**********************************************************/
TEST_F(TestFastBufferdStream,puInt)
{
	EXPECT_EQ("",out->str());
	*bufferd << 10;
	EXPECT_EQ("",out->str());
	bufferd->flush();
	EXPECT_EQ("10",out->str());
}

/**********************************************************/
TEST_F(TestFastBufferdStream,putPtr)
{
	EXPECT_EQ("",out->str());
	*bufferd << (void*)0x10;
	EXPECT_EQ("",out->str());
	bufferd->flush();
	EXPECT_EQ("0x10",out->str());
}

/**********************************************************/
TEST_F(TestFastBufferdStream,putFloat)
{
	EXPECT_EQ("",out->str());
	*bufferd << 0.5;
	EXPECT_EQ("",out->str());
	bufferd->flush();
	EXPECT_EQ("0.5",out->str());
}

/**********************************************************/
TEST_F(TestFastBufferdStream,putDouble)
{
	EXPECT_EQ("",out->str());
	*bufferd << (double)0.5;
	EXPECT_EQ("",out->str());
	bufferd->flush();
	EXPECT_EQ("0.5",out->str());
}

/**********************************************************/
TEST_F(TestFastBufferdStream,putUnsignedLong)
{
	EXPECT_EQ("",out->str());
	*bufferd << (unsigned long)10;
	EXPECT_EQ("",out->str());
	bufferd->flush();
	EXPECT_EQ("10",out->str());
}

/**********************************************************/
TEST_F(TestFastBufferdStream,putLong)
{
	EXPECT_EQ("",out->str());
	*bufferd << (long)-10l;
	EXPECT_EQ("",out->str());
	bufferd->flush();
	EXPECT_EQ("-10",out->str());
}

/**********************************************************/
TEST_F(TestFastBufferdStream,putStreamObj)
{
	EXPECT_EQ("",out->str());
	*bufferd << *this;
	EXPECT_EQ("TestFastBufferdStream",out->str());
	bufferd->flush();
	EXPECT_EQ("TestFastBufferdStream",out->str());
}

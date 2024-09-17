/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.2
*    DATE     : 09/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/tests/TestElfReader.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2020 - 2024
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include <tools/ELFReader.hpp>
#include <common/SimpleAllocator.hpp>

/**********************************************************/
using namespace MALT;

/**********************************************************/
static const char CST_BINARY_FILE[] = TEST_BIN_DIR "/../../tests/simple-case-no-finstr"; 

/**********************************************************/
TEST(ElfReader,constructor)
{
	ElfReader reader(CST_BINARY_FILE);
}

/**********************************************************/
bool hasVariable(ElfGlobalVariableVector & vars,std::string name,size_t size,bool tls)
{
	for (ElfGlobalVariableVector::const_iterator it = vars.begin() ; it != vars.end() ; ++it)
	{
		//fprintf(stderr,"%s -- %zu -- %d\n",it->name.c_str(),it->size,it->tls);
		if (it->name == name && it->size == size && it->tls == tls)
			return true;
	}
	
	return false;
}

/**********************************************************/
TEST(ElfReader,loadSimpleCaseGlobVars)
{
	ElfReader reader(CST_BINARY_FILE);
	ElfGlobalVariableVector vars;
	reader.loadGlobalVariables(vars);
	
	if (ElfReader::hasLibElf())
	{
		EXPECT_TRUE(hasVariable(vars,"gblArray",1024 * sizeof(int),false));
		EXPECT_TRUE(hasVariable(vars,"_ZL14gblStaticArray",1024 * sizeof(int),false));
		EXPECT_TRUE(hasVariable(vars,"_ZL9gblString",25,false));
		EXPECT_TRUE(hasVariable(vars,"tlsArray",1024 * sizeof(int),true));
	}
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

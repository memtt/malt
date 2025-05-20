/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
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
static const char CST_THIS_BIN_FILE[] = TEST_BIN_DIR "/TestElfReader";
static char gbl_large_var_200MB[200UL*1024*1024UL];
extern char gbl_large_var_200MB_dyn[200UL*1024*1024UL];

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
		EXPECT_TRUE(hasVariable(vars,"gblStaticArray",1024 * sizeof(int),false));
		EXPECT_TRUE(hasVariable(vars,"gblString",25,false));
		EXPECT_TRUE(hasVariable(vars,"tlsArray",1024 * sizeof(int),true));
	}
}

/**********************************************************/
TEST(ElfReader,getPhysAddr_exe)
{
	//init half
	memset(gbl_large_var_200MB, 0, sizeof(gbl_large_var_200MB) / 2);

	//load vars
	ElfReader reader(CST_THIS_BIN_FILE);
	ElfGlobalVariableVector vars;
	reader.loadGlobalVariables(vars);
	const ElfGlobalVariable & var = reader.getVarByName(vars, "gbl_large_var_200MB");
	LinuxProcMapReader procMapReader;
	procMapReader.load();
	EXPECT_EQ(gbl_large_var_200MB, reader.getInMemAddr(procMapReader, var));
}

/**********************************************************/
TEST(ElfReader,getPhysAddr_so)
{
	//init half
	memset(gbl_large_var_200MB_dyn, 0, sizeof(gbl_large_var_200MB_dyn) / 2);

	//load vars
	ElfReader reader(CST_THIS_BIN_FILE);
	ElfGlobalVariableVector vars;
	reader.loadGlobalVariables(vars);
	const ElfGlobalVariable & var = reader.getVarByName(vars, "gbl_large_var_200MB_dyn");
	LinuxProcMapReader procMapReader;
	procMapReader.load();
	EXPECT_EQ(gbl_large_var_200MB_dyn, reader.getInMemAddr(procMapReader, var));
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

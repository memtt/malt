/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 05/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/tools/tests/TestNMCmdReader.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2022
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include "../NMCmdReader.hpp"
#include "../ELFReader.hpp"

/**********************************************************/
using namespace MALT;

/**********************************************************/
int gblVariable = 10;

/**********************************************************/
TEST(TestNMCmdReader, constructor)
{
	NMCmdReader reader;
}

/**********************************************************/
bool hasVar(ElfGlobalVariableVector & vars, const std::string & name, bool acceptNoSource)
{
	for (auto & it : vars) {
		if (it.name == name) {
			if (it.sourceFile.empty() == false && it.line > 0)
				return true;
			else
				return acceptNoSource;
		}
	}
	return false;
}

/**********************************************************/
TEST(TestNMCmdReader, load)
{
	//extract vars
	ElfReader elfReader(TEST_BIN_DIR "/TestNMCmdReader");
	ElfGlobalVariableVector vars;
	elfReader.loadGlobalVariables(vars);

	//terminate the test if we do not have libelf
	if (ElfReader::hasLibElf() == false)
		return;

	//load & extract source infos
	NMCmdReader nmReader;
	nmReader.load(TEST_BIN_DIR "/TestNMCmdReader");
	nmReader.findSourcesAndDemangle(vars);

	//check
	EXPECT_TRUE(hasVar(vars, "gblVariable", false));
}

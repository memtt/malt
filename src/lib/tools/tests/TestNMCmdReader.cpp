/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include "../NMCmdReader.hpp"
#include "../ELFReader.hpp"

/***************** USING NAMESPACE ******************/
using namespace MALT;

/********************* GLOBALS **********************/
int gblVariable = 10;

/*******************  FUNCTION  *********************/
TEST(TestNMCmdReader, constructor)
{
	NMCmdReader reader;
}

/*******************  FUNCTION  *********************/
bool hasVar(ElfGlobalVariableVector & vars, const std::string & name, bool acceptNoSource)
{
	for (auto & it : vars) {
		if (it.name == name) {
			if (it.file.empty() == false && it.line > 0)
				return true;
			else
				return acceptNoSource;
		}
	}
	return false;
}

/*******************  FUNCTION  *********************/
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

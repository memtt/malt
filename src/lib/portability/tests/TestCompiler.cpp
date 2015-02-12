/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <portability/Compiler.hpp>

/***************** USING NAMESPACE ******************/
using namespace MATT;

/*******************  FUNCTION  *********************/
TEST(Compiler,constructor)
{
	EXPECT_EQ("MATT::OSUnix::getMemoryUsage()::gblMeminfoMutex",Compiler::demangleCppNames("_ZZN4MATT6OSUnix14getMemoryUsageEvE15gblMeminfoMutex"));
}


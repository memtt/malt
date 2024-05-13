/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.2
*    DATE     : 06/2023
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/Backtrace.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/
/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <common/Options.hpp>
#include "Backtrace.hpp"
#include "BacktraceGlibc.hpp"
#include "BacktraceLibunwind.hpp"

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*******************  FUNCTION  *********************/
int Backtrace::backtrace ( void** buffer, int maxDepth )
{
	if (getOptions().stackLibunwind)
		return BacktraceLibunwind::backtrace(buffer,maxDepth);
	else
		return BacktraceGlibc::backtrace(buffer,maxDepth);
}

}

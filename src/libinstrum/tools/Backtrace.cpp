/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 11/2014
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/tools/Backtrace.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014
***********************************************************/

/**********************************************************/
#include <common/Options.hpp>
#include "Backtrace.hpp"
#include "BacktraceGlibc.hpp"
#include "BacktraceLibunwind.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
int Backtrace::backtrace ( void** buffer, int maxDepth )
{
	if (getOptions().stackLibunwind)
		return BacktraceLibunwind::backtrace(buffer,maxDepth);
	else
		return BacktraceGlibc::backtrace(buffer,maxDepth);
}

}

/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/tools/Backtrace.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014 - 2026
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
	if (getOptions().stack.libunwind)
		return BacktraceLibunwind::backtrace(buffer,maxDepth);
	else
		return BacktraceGlibc::backtrace(buffer,maxDepth);
}

}

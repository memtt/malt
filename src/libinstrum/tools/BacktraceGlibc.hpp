/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2014
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/tools/BacktraceGlibc.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

#ifndef MALT_BACKTRACE_GLIBC_HPP
#define MALT_BACKTRACE_GLIBC_HPP

/**********************************************************/
//GNU specific for glibc backtrace
#include <execinfo.h>

/**********************************************************/
namespace MALT
{

/**********************************************************/
class BacktraceGlibc
{
	public:
		static int backtrace(void** buffer,int maxDepth) { return ::backtrace(buffer,maxDepth);}
};

}

#endif //MALT_BACKTRACE_GLIBC_HPP

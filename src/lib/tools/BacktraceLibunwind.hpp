/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.5
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/BacktraceLibunwind.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2024
***********************************************************/

#ifndef MALT_BACKTRACE_LIBUNWIND_HPP
#define MALT_BACKTRACE_LIBUNWIND_HPP

/**********************************************************/
namespace MALT
{

/**********************************************************/
class BacktraceLibunwind
{
	public:
		static int backtrace(void** buffer,int maxDepth);
};

}

#endif //MALT_BACKTRACE_LIBUNWIND_HPP

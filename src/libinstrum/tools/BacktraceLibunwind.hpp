/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 11/2014
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/tools/BacktraceLibunwind.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
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

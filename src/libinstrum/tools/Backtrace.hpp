/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 11/2014
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/tools/Backtrace.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014
***********************************************************/

#ifndef MALT_BACKTRACE_HPP
#define MALT_BACKTRACE_HPP

/**********************************************************/
namespace MALT
{

/**********************************************************/
class Backtrace
{
	public:
		static int backtrace(void** buffer,int maxDepth);
};

}

#endif //MALT_BACKTRACE_HPP

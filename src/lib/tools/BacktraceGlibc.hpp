/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_BACKTRACE_GLIBC_HPP
#define MALT_BACKTRACE_GLIBC_HPP

/********************  HEADERS  *********************/
//GNU specific for glibc backtrace
#include <execinfo.h>

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*********************  CLASS  **********************/
class BacktraceGlibc
{
	public:
		static int backtrace(void** buffer,int maxDepth) { return ::backtrace(buffer,maxDepth);}
};

}

#endif //MALT_BACKTRACE_GLIBC_HPP

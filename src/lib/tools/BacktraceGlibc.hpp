/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_BACKTRACE_GLIBC_HPP
#define MATT_BACKTRACE_GLIBC_HPP

/********************  HEADERS  *********************/
//GNU specific for glibc backtrace
#include <execinfo.h>

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  CLASS  **********************/
class BacktraceGlibc
{
	public:
		static int backtrace(void** buffer,int maxDepth) { return ::backtrace(buffer,maxDepth);}
};

}

#endif //MATT_BACKTRACE_GLIBC_HPP

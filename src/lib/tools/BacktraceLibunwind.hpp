/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_BACKTRACE_LIBUNWIND_HPP
#define MATT_BACKTRACE_LIBUNWIND_HPP

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  CLASS  **********************/
class BacktraceLibunwind
{
	public:
		static int backtrace(void** buffer,int maxDepth);
};

}

#endif //MATT_BACKTRACE_LIBUNWIND_HPP

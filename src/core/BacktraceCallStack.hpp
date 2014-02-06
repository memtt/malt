/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_BACKTRACE_BACKTRACE_HPP
#define MATT_BACKTRACE_BACKTRACE_HPP

/********************  HEADERS  *********************/
#include "Stack.h"
#include "SymbolResolver.hpp"
#include "CallStackInfo.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  CLASS  **********************/
class BacktraceCallStack : public Stack
{
	public:
		BacktraceCallStack(void);
		void loadCurrentStack(void);
};

}

#endif //MATT_BACKTRACE_BACKTRACE_HPP

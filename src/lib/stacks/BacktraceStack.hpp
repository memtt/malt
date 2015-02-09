/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_BACKTRACE_STACK_HPP
#define MATT_BACKTRACE_STACK_HPP

/********************  HEADERS  *********************/
//internal stacks
#include <stacks/Stack.hpp>
//internal core
// #include <core/SymbolSolver.hpp>
// #include <core/CallStackInfo.hpp>

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  CLASS  **********************/
/**
 * Implement a specific class to provide backtrace integration on top of our internal
 * stack representation.
**/
class BacktraceStack : public Stack
{
	public:
		BacktraceStack(void);
		void loadCurrentStack(void);
};

}

#endif //MATT_BACKTRACE_STACK_HPP

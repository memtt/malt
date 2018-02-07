/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.1.0-dev
             DATE     : 02/2018
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_BACKTRACE_STACK_HPP
#define MALT_BACKTRACE_STACK_HPP

/********************  HEADERS  *********************/
//internal stacks
#include <stacks/Stack.hpp>
//internal core
#include <core/SymbolSolver.hpp>
#include <core/CallStackInfo.hpp>

/*******************  NAMESPACE  ********************/
namespace MALT
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

#endif //MALT_BACKTRACE_STACK_HPP

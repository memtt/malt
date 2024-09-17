/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.2
*    DATE     : 06/2023
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/stack-tree/from-v2/BacktraceStack.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2015
***********************************************************/

#ifndef MALTV2_BACKTRACE_STACK_HPP
#define MALTV2_BACKTRACE_STACK_HPP

/**********************************************************/
//internal stacks
#include "Stack.hpp"
//internal core
// #include <core/SymbolSolver.hpp>
// #include <core/CallStackInfo.hpp>

/**********************************************************/
namespace MALTV2
{

/**********************************************************/
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

#endif //MALTV2_BACKTRACE_STACK_HPP

/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/stack-tree/from-v2/BacktraceStack.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2015 - 2024
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
		~BacktraceStack(void);
		void loadCurrentStack(void);
	protected:
		virtual void onGrow(size_t newSize);
	private:
		void ** stackBuffer{nullptr};
};

}

#endif //MALTV2_BACKTRACE_STACK_HPP

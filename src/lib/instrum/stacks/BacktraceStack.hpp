/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/stacks/BacktraceStack.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

#ifndef MALT_BACKTRACE_STACK_HPP
#define MALT_BACKTRACE_STACK_HPP

/**********************************************************/
//internal stacks
#include <stacks/Stack.hpp>
//internal core
#include <core/SymbolSolver.hpp>
#include <core/CallStackInfo.hpp>

/**********************************************************/
namespace MALT
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
		int getBactraceSkipOptimDelta(void);
	protected:
		virtual void onGrow(size_t newSize);
	private:
		void ** stackBuffer{nullptr};
};

}

#endif //MALT_BACKTRACE_STACK_HPP

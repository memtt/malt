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

#ifndef MALT_BACKTRACE_PYTHON_STACK_HPP
#define MALT_BACKTRACE_PYTHON_STACK_HPP

/**********************************************************/
//internal stacks
#include <stacks/Stack.hpp>
//internal core
#include <core/SymbolSolver.hpp>
#include <core/CallStackInfo.hpp>
#include <core/PythonSymbolTracker.hpp>
//python
#include <Python.h>

/**********************************************************/
namespace MALT
{

/**********************************************************/
/**
 * Implement a specific class to provide backtrace integration on top of our internal
 * stack representation.
**/
class BacktracePythonStack : public Stack
{
	public:
		BacktracePythonStack(PythonSymbolTracker & pythonSymbolTracker);
		~BacktracePythonStack(void);
		void loadCurrentStack(void);
		LangAddress getCurrentFrameAddr(void);
		PyFrameObject * loadCurrentFrame(void);
	private:
		PythonSymbolTracker & pythonSymbolTracker;
};

}

#endif //MALT_BACKTRACE_PYTHON_STACK_HPP

/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/stacks/BacktracePythonStack.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
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
#ifdef MALT_HAVE_PYTHON
	#include <Python.h>
#endif //MALT_HAVE_PYTHON

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

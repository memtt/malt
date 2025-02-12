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

/**********************************************************/
namespace MALT
{

typedef std::basic_string<char, std::char_traits<char>, STLInternalAllocator<char> > StringNoFree;

/**********************************************************/
struct PythonCallSite
{
	const char * file;
	const char * function;
	int line;
};

/**********************************************************/
bool operator<(const PythonCallSite & a, const PythonCallSite & b);

/**********************************************************/
typedef std::map<PythonCallSite, size_t, std::less<PythonCallSite>, STLInternalAllocator<std::pair<PythonCallSite,size_t> > > PythonStrCallSiteMap;

/**********************************************************/
/**
 * Implement a specific class to provide backtrace integration on top of our internal
 * stack representation.
**/
class BacktracePythonStack : public Stack
{
	public:
		BacktracePythonStack(void);
		~BacktracePythonStack(void);
		void loadCurrentStack(void);
		void registerSymbolResolution(SymbolSolver & solver) const;
	private:
		PythonStrCallSiteMap siteMap;
		size_t nextIndex{10};
};

}

#endif //MALT_BACKTRACE_PYTHON_STACK_HPP

/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.2
*    DATE     : 06/2023
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/core/ValgrindOutput.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

#ifndef MALT_VALGRIND_OUTPUT_HPP
#define MALT_VALGRIND_OUTPUT_HPP

/**********************************************************/
#include <cstdlib>
#include <map>
#include <string>
#include "SimpleCallStackNode.hpp"
#include "SymbolSolver.hpp"

namespace MALT
{

/**********************************************************/
typedef std::map<void*,CallStackInfo> ValgrindCalleeMap;

/**********************************************************/
struct ValgrindCaller
{
	CallStackInfo info;
	ValgrindCalleeMap callees;
};

/**********************************************************/
typedef std::map<void*,ValgrindCaller> ValgrindCallerMap;

/**********************************************************/
class ValgrindOutput
{
	public:
		void pushStackInfo(SimpleCallStackNode& stackNode, const SymbolSolver& symbols);
		void pushStackInfo(const Stack & stack,const CallStackInfo & info, const SymbolSolver& symbols);
		void writeAsCallgrind(const std::string & filename,const SymbolSolver& dic);
		void writeAsCallgrind(std::ostream & out, const SymbolSolver& dic);
	protected:
		void writeLocation(std::ostream& out, const SymbolSolver& dic, const CallSite * site, void * addr, bool call);
		static bool isNewOperator(const SymbolSolver& symbols, void* addr);
	private:
		ValgrindCallerMap callers;
};

}

#endif //MALT_VALGRIND_OUTPUT_HPP

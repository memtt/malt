/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.0
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/core/ValgrindOutput.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
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
typedef std::map<LangAddress,CallStackInfo> ValgrindCalleeMap;

/**********************************************************/
struct ValgrindCaller
{
	CallStackInfo info;
	ValgrindCalleeMap callees;
};

/**********************************************************/
typedef std::map<LangAddress,ValgrindCaller> ValgrindCallerMap;

/**********************************************************/
class ValgrindOutput
{
	public:
		void pushStackInfo(SimpleCallStackNode& stackNode, const SymbolSolver& symbols);
		void pushStackInfo(const Stack & stack,const CallStackInfo & info, const SymbolSolver& symbols);
		void writeAsCallgrind(const std::string & filename,const SymbolSolver& dic);
		void writeAsCallgrind(std::ostream & out, const SymbolSolver& dic);
	protected:
		void writeLocation(std::ostream& out, const SymbolSolver& dic, const CallSite * site, LangAddress addr, bool call);
		static bool isNewOperator(const SymbolSolver& symbols, LangAddress addr);
	private:
		ValgrindCallerMap callers;
};

}

#endif //MALT_VALGRIND_OUTPUT_HPP

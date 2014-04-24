/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_VALGRIND_OUTPUT_HPP
#define MATT_VALGRIND_OUTPUT_HPP

/********************  HEADERS  *********************/
#include <cstdlib>
#include <map>
#include <string>
#include "SimpleCallStackNode.hpp"
#include "SymbolResolver.hpp"

namespace MATT
{

/*********************  TYPES  **********************/
typedef std::map<void*,CallStackInfo> ValgrindCalleeMap;

/********************  STRUCT  **********************/
struct ValgrindCaller
{
	CallStackInfo info;
	ValgrindCalleeMap callees;
};

/*********************  TYPES  **********************/
typedef std::map<void*,ValgrindCaller> ValgrindCallerMap;

/*********************  CLASS  **********************/
class ValgrindOutput
{
	public:
		void pushStackInfo(SimpleCallStackNode& stackNode, const SymbolResolver& symbols);
		void pushStackInfo(const Stack & stack,const CallStackInfo & info, const SymbolResolver& symbols);
		void writeAsCallgrind(const std::string & filename,const SymbolResolver& dic);
		void writeAsCallgrind(std::ostream & out, const SymbolResolver& dic);
	protected:
		void writeLocation(std::ostream& out, const SymbolResolver& dic, const CallSite * site, void * addr, bool call);
		static bool isNewOperator(const SymbolResolver& symbols, void* addr);
	private:
		ValgrindCallerMap callers;
};

}

#endif //MATT_VALGRIND_OUTPUT_HPP

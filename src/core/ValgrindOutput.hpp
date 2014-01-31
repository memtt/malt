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
		void pushStackInfo(SimpleCallStackNode & stackNode);
		void writeAsCallgrind(const std::string & filename,const FuncNameDic& dic);
	protected:
		void writeAsCallgrind(std::ostream & out, const FuncNameDic& dic);
		void writeLocation(std::ostream& out, const FuncNameDic& dic, const CallSite * site, void * addr, bool call);
	private:
		ValgrindCallerMap callers;
};

};

#endif //MATT_VALGRIND_OUTPUT_HPP

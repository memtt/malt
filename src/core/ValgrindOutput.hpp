#ifndef ATT_VALGRIND_OUTPUT_HPP
#define ATT_VALGRIND_OUTPUT_HPP

/********************  HEADERS  *********************/
#include <cstdlib>
#include <map>
#include <string>
#include "SimpleCallStackNode.hpp"

namespace ATT
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
		void writeAsCallgrind(const std::string & filename);
	protected:
		void writeAsCallgrind(std::ostream & out);
	private:
		ValgrindCallerMap callers;
};

};

#endif //ATT_VALGRIND_OUTPUT_HPP

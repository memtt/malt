#ifndef SIMPLE_BACKTRACE_STORAGE_H
#define SIMPLE_BACKTRACE_STORAGE_H

/********************  HEADERS  *********************/
//extern
#include <map>
#include <vector>
#include <ostream>
#include <json/TypeToJson.h>
//locals
#include "FuncNameDic.h"
#include "BacktraceCallStack.h"
#include "SimpleCallStackNode.h"

/*******************  FUNCTION  *********************/
typedef std::vector<SimpleCallStackNode*> SimpleBacktraceVector;
typedef std::map<StackHash,SimpleBacktraceVector> SimpleBacktraceVectorMap;

/*********************  CLASS  **********************/
class SimpleStackTracer
{
	public:
		SimpleStackTracer(void);
		~SimpleStackTracer(void);
		SimpleCallStackNode& getBacktraceInfo(void);
		SimpleCallStackNode& getBacktraceInfo(const Stack & stack);
		void resolveSymbols(FuncNameDic & dic) const;
	public:
		friend std::ostream & operator << (std::ostream & out,const SimpleStackTracer & tracer);
		friend void typeToJson(htopml::JsonState & json,std::ostream& stream, const SimpleStackTracer & value);
	private:
		SimpleBacktraceVectorMap callmaps;
};

#endif //SIMPLE_BACKTRACE_STORAGE_H

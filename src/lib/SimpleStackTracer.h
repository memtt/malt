#ifndef SIMPLE_BACKTRACE_STORAGE_H
#define SIMPLE_BACKTRACE_STORAGE_H

/********************  HEADERS  *********************/
#include <map>
#include <vector>
#include <ostream>
#include "FuncNameDic.h"
#include "SimpleCallStack.h"
#include <json/TypeToJson.h>

/*******************  FUNCTION  *********************/
typedef std::vector<SimpleCallStack*> SimpleBacktraceVector;
typedef std::map<SimpleBacktraceHash,SimpleBacktraceVector> SimpleBacktraceVectorMap;

/*********************  CLASS  **********************/
class SimpleStackTracer
{
	public:
		SimpleStackTracer(void);
		~SimpleStackTracer(void);
		SimpleCallStack & getBacktraceInfo(void ** callStack,int size);
		void resolveSymbols(FuncNameDic & dic) const;
	public:
		friend std::ostream & operator << (std::ostream & out,const SimpleStackTracer & tracer);
		friend void typeToJson(htopml::JsonState & json,std::ostream& stream, const SimpleStackTracer & value);
	private:
		SimpleBacktraceVectorMap callmaps;
};

#endif //SIMPLE_BACKTRACE_STORAGE_H

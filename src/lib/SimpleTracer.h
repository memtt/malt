#ifndef SIMPLE_BACKTRACE_STORAGE_H
#define SIMPLE_BACKTRACE_STORAGE_H

/********************  HEADERS  *********************/
#include <map>
#include <vector>
#include <ostream>
#include "SimpleCallStack.h"
#include "FuncNameDic.h"
#include <json/TypeToJson.h>

/*******************  FUNCTION  *********************/
typedef std::vector<SimpleCallStack*> SimpleBacktraceVector;
typedef std::map<SimpleBacktraceHash,SimpleBacktraceVector> SimpleBacktraceVectorMap;

/*********************  CLASS  **********************/
class SimpleTracer
{
	public:
		SimpleTracer(void);
		~SimpleTracer(void);
		SimpleCallStack & getBacktraceInfo(void ** callStack,int size);
		void resolveSymbols(FuncNameDic & dic) const;
	public:
		friend std::ostream & operator << (std::ostream & out,const SimpleTracer & tracer);
		friend void typeToJson(htopml::JsonState & json,std::ostream& stream, const SimpleTracer & value);
	private:
		SimpleBacktraceVectorMap callmaps;
};

#endif //SIMPLE_BACKTRACE_STORAGE_H

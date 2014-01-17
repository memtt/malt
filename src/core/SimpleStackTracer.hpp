#ifndef ATT_SIMPLE_BACKTRACE_STORAGE_HPP
#define ATT_SIMPLE_BACKTRACE_STORAGE_HPP

/********************  HEADERS  *********************/
//extern
#include <map>
#include <vector>
#include <ostream>
// #include <json/TypeToJson.h>
//locals
#include "FuncNameDic.hpp"
#include "BacktraceCallStack.hpp"
#include "SimpleCallStackNode.hpp"

/*******************  NAMESPACE  ********************/
namespace ATT
{

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
		size_t count;
};

}

#endif //ATT_SIMPLE_BACKTRACE_STORAGE_HPP

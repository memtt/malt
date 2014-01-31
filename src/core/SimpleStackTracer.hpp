/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_SIMPLE_BACKTRACE_STORAGE_HPP
#define MATT_SIMPLE_BACKTRACE_STORAGE_HPP

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
#include "ValgrindOutput.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
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
		void resolveSymbols(void);
		void fillValgrindOut(ValgrindOutput & out) const;
		const FuncNameDic & getNameDic(void) const;
	public:
		friend std::ostream & operator << (std::ostream & out,const SimpleStackTracer & tracer);
		friend void convertToJson(htopml::JsonState & json, const SimpleStackTracer & value);
	private:
		SimpleBacktraceVectorMap callmaps;
		FuncNameDic dic;
		size_t count;
};

}

#endif //MATT_SIMPLE_BACKTRACE_STORAGE_HPP

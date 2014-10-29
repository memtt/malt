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
#include "SymbolSolver.hpp"
#include <stacks/BacktraceStack.hpp>
#include "SimpleCallStackNode.hpp"
#include "ValgrindOutput.hpp"
#include <common/STLInternalAllocator.hpp>
#include <common/Array.hpp>

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*******************  FUNCTION  *********************/
//typedef std::vector<SimpleCallStackNode*> SimpleBacktraceVector;
typedef std::vector<SimpleCallStackNode*,STLInternalAllocator<SimpleCallStackNode*> > SimpleBacktraceVector;
// typedef MATT::Array<SimpleCallStackNode*> SimpleBacktraceVector;
// typedef std::map<StackHash,SimpleBacktraceVector> SimpleBacktraceVectorMap;
typedef std::map<StackHash,SimpleBacktraceVector,std::less<StackHash>,STLInternalAllocator<std::pair<StackHash,SimpleBacktraceVector> > > SimpleBacktraceVectorMap;

/*********************  CLASS  **********************/
class SimpleStackTracker
{
	public:
		SimpleStackTracker(void);
		~SimpleStackTracker(void);
		SimpleCallStackNode& getBacktraceInfo(const Stack& stack, int skipDepth = 0);
		void solveSymbols(SymbolSolver & symbolResolver);
		void fillValgrindOut(MATT::ValgrindOutput& out, MATT::SymbolSolver& symbolResolver) const;
	public:
		friend std::ostream & operator << (std::ostream & out,const SimpleStackTracker & tracer);
		friend void convertToJson(htopml::JsonState & json, const SimpleStackTracker & value);
	private:
		SimpleBacktraceVectorMap callmaps;
		size_t count;
};

}

#endif //MATT_SIMPLE_BACKTRACE_STORAGE_HPP

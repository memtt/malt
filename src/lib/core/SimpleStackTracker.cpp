/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <json/JsonState.h>
#include <common/CodeTiming.hpp>
#include "SimpleStackTracker.hpp"

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*******************  FUNCTION  *********************/
/**
 * Constructor of the stack tracker.
**/
SimpleStackTracker::SimpleStackTracker(void )
{
	this->count = 0;
}

/*******************  FUNCTION  *********************/
/**
 * Destructor of the stack tracker to clear the memory.
**/
SimpleStackTracker::~SimpleStackTracker(void )
{
	for (SimpleBacktraceVectorMap::const_iterator itMap = callmaps.begin() ; itMap != callmaps.end() ; ++itMap)
	{
		const SimpleBacktraceVector & vec = itMap->second;
		for (SimpleBacktraceVector::const_iterator it = vec.begin() ; it != vec.end() ; ++it)
			delete *it;
	}
}

/*******************  FUNCTION  *********************/
/**
 * This function is used to get a reference to the call stack node from the call
 * stack.
 * @param stack Reference to the call stack to be used to identify the profiling
 * infos.
 * @param skipDepth Skip part of the stack before doing the search.
**/
SimpleCallStackNode& SimpleStackTracker::getBacktraceInfo( const Stack& stack , int skipDepth )
{
	assert(stack.isValid());
	
	//calc current hash
	StackHash hash;
	CODE_TIMING("stackHash",hash = stack.hash(skipDepth));

	//search in current vector
	static CodeTiming tmap("searchInMap");
	ticks start = tmap.start();
	SimpleBacktraceVector & vec = callmaps[hash];
	tmap.end(start);

	//loop in vector to find the good one
	static CodeTiming tvec("searchInSubVect");
	start = tvec.start();
	SimpleBacktraceVector::iterator resIt = vec.end();
	for (SimpleBacktraceVector::iterator it = vec.begin() ; it != vec.end() ; ++it)
		if (Stack::partialCompare((*it)->getCallStack(),0,stack,skipDepth))
			resIt = it;
	tvec.end(start);

	count++;

	//if not found create and add
	if (resIt == vec.end())
	{
		SimpleCallStackNode * newEntry = new SimpleCallStackNode(stack,skipDepth);
		CODE_TIMING("insertInVec",vec.push_back(newEntry));
		if (vec.size() > 100)
			fprintf(stderr,"warning get too much multiple in map sub-array, it may hust performances : %zu\n",vec.size());
		return *newEntry;
	}else {
		return **resIt;
	}
}

/*******************  FUNCTION  *********************/
/**
 * Stream operator do to help debugging.
 * @param out Reference to the output stream operator.
 * @param tracer Reference to the tracer to dump.
 * @return Reference to the output stream after used.
**/
std::ostream& operator<<(std::ostream& out, const SimpleStackTracker& tracer)
{
	for (SimpleBacktraceVectorMap::const_iterator itMap = tracer.callmaps.begin() ; itMap != tracer.callmaps.end() ; ++itMap)
	{
		const SimpleBacktraceVector & vec = itMap->second;
		for (SimpleBacktraceVector::const_iterator it = vec.begin() ; it != vec.end() ; ++it)
			out << **it << std::endl;
	}
	
	return out;
}

/*******************  FUNCTION  *********************/
/**
 * Use the symbol solver to resolve the symbols.
 * @param symbolResolver Reference to the symbol resolver to be used on all the
 * symbols tracked by the tracker.
**/
void SimpleStackTracker::solveSymbols(SymbolSolver & symbolResolver)
{
	for (SimpleBacktraceVectorMap::const_iterator itMap = callmaps.begin() ; itMap != callmaps.end() ; ++itMap)
	{
		const SimpleBacktraceVector & vec = itMap->second;
		for (SimpleBacktraceVector::const_iterator it = vec.begin() ; it != vec.end() ; ++it)
			(*it)->getCallStack().solveSymbols(symbolResolver);
	}
}

/*******************  FUNCTION  *********************/
/**
 * Convert the tracker state into json format to create the final profile file.
 * @param json Reference to the json state to make the conversion.
 * @param value Reference to the tracker to convert.
**/
void convertToJson(htopml::JsonState& json, const SimpleStackTracker& value)
{
	json.openStruct();
	json.openFieldArray("stats");
	for (SimpleBacktraceVectorMap::const_iterator itMap = value.callmaps.begin() ; itMap != value.callmaps.end() ; ++itMap)
	{
		const SimpleBacktraceVector & vec = itMap->second;
		for (SimpleBacktraceVector::const_iterator it = vec.begin() ; it != vec.end() ; ++it)
			json.printValue(**it);
	}
	json.closeFieldArray("stats");
	json.printField("count",value.count);
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
/**
 * Function usd to convert the tracker into the valgrind format.
 * @param out Reference to the valgrind converter to fill it states with the
 * tracked stacks.
 * @param symbolResolber Reference to the symbolResolver to be used by the 
 * valgrind converter.
**/
void SimpleStackTracker::fillValgrindOut(ValgrindOutput& out,SymbolSolver & symbolResolver) const
{
	for (SimpleBacktraceVectorMap::const_iterator itMap = callmaps.begin() ; itMap != callmaps.end() ; ++itMap)
	{
		const SimpleBacktraceVector & vec = itMap->second;
		for (SimpleBacktraceVector::const_iterator it = vec.begin() ; it != vec.end() ; ++it)
			out.pushStackInfo(**it,symbolResolver);
	}
}

}

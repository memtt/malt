/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.0-dev
             DATE     : 08/2019
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
SimpleStackTracker::SimpleStackTracker(void )
{
	this->count = 0;
}

/*******************  FUNCTION  *********************/
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
void SimpleStackTracker::fillValgrindOut(ValgrindOutput& out,SymbolSolver & symbolResolver) const
{
	for (SimpleBacktraceVectorMap::const_iterator itMap = callmaps.begin() ; itMap != callmaps.end() ; ++itMap)
	{
		const SimpleBacktraceVector & vec = itMap->second;
		for (SimpleBacktraceVector::const_iterator it = vec.begin() ; it != vec.end() ; ++it)
			out.pushStackInfo(**it,symbolResolver);
	}
}

/*******************  FUNCTION  *********************/


}

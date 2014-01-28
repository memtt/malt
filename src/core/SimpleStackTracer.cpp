/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <json/JsonState.h>
#include <common/CodeTiming.hpp>
#include "SimpleStackTracer.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*******************  FUNCTION  *********************/
SimpleStackTracer::SimpleStackTracer(void )
{
	this->count = 0;
}

/*******************  FUNCTION  *********************/
SimpleStackTracer::~SimpleStackTracer(void )
{
	for (SimpleBacktraceVectorMap::const_iterator itMap = callmaps.begin() ; itMap != callmaps.end() ; ++itMap)
	{
		const SimpleBacktraceVector & vec = itMap->second;
		for (SimpleBacktraceVector::const_iterator it = vec.begin() ; it != vec.end() ; ++it)
			delete *it;
	}
}

/*******************  FUNCTION  *********************/
SimpleCallStackNode& SimpleStackTracer::getBacktraceInfo( const Stack& stack )
{
	assert(stack.isValid());
	
	//calc current hash
	StackHash hash;
	CODE_TIMING("stackHash",hash = stack.hash());

	//search in current vector
	static CodeTiming tmap("searchInMap");
	tmap.start();
	SimpleBacktraceVector & vec = callmaps[hash];
	tmap.end();

	//loop in vector to find the good one
	static CodeTiming tvec("searchInSubVect");
	tvec.start();
	SimpleBacktraceVector::iterator resIt = vec.end();
	for (SimpleBacktraceVector::iterator it = vec.begin() ; it != vec.end() ; ++it)
		if ((*it)->getCallStack() == stack)
			resIt = it;
	tvec.end();

	count++;

	//if not found create and add
	if (resIt == vec.end())
	{
		SimpleCallStackNode * newEntry = new SimpleCallStackNode(stack);
		CODE_TIMING("insertInVec",vec.push_back(newEntry));
		if (vec.size() > 100)
			fprintf(stderr,"warning get too much multiple in map sub-array, it may hust performances : %lu\n",vec.size());
		return *newEntry;
	}else {
		return **resIt;
	}
}

/*******************  FUNCTION  *********************/
std::ostream& operator<<(std::ostream& out, const SimpleStackTracer& tracer)
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
void SimpleStackTracer::resolveSymbols(FuncNameDic& dic) const
{
	for (SimpleBacktraceVectorMap::const_iterator itMap = callmaps.begin() ; itMap != callmaps.end() ; ++itMap)
	{
		const SimpleBacktraceVector & vec = itMap->second;
		for (SimpleBacktraceVector::const_iterator it = vec.begin() ; it != vec.end() ; ++it)
			(*it)->getCallStack().resolveSymbols(dic);
	}
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const SimpleStackTracer& value)
{
	FuncNameDic dic;
// 	CODE_TIMING("resolveSymbols",value.resolveSymbols(dic));

	json.openStruct();
	json.printField("sites",dic);
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
void SimpleStackTracer::fillValgrindOut(ValgrindOutput& out) const
{
	for (SimpleBacktraceVectorMap::const_iterator itMap = callmaps.begin() ; itMap != callmaps.end() ; ++itMap)
	{
		const SimpleBacktraceVector & vec = itMap->second;
		for (SimpleBacktraceVector::const_iterator it = vec.begin() ; it != vec.end() ; ++it)
			out.pushStackInfo(**it);
	}
}

}

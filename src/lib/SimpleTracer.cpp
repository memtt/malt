/********************  HEADERS  *********************/
#include <cassert>
#include <cstdlib>
#include "SimpleTracer.h"

/*******************  FUNCTION  *********************/
SimpleTracer::SimpleTracer(void )
{

}

/*******************  FUNCTION  *********************/
SimpleTracer::~SimpleTracer(void )
{

}

/*******************  FUNCTION  *********************/
SimpleCallStack& SimpleTracer::getBacktraceInfo(void** callStack, int size)
{
	//errors
	assert(callStack != NULL);
	assert(size > 0);

	//calc current hash
	SimpleBacktraceHash hash = SimpleCallStack::getSimpleHash(callStack,size);

	//search in current vector
	SimpleBacktraceVector & vec = callmaps[hash];

	//loop in vector to find the good one
	SimpleBacktraceVector::iterator resIt = vec.end();
	for (SimpleBacktraceVector::iterator it = vec.begin() ; it != vec.end() ; ++it)
		if ((*it)->equal(callStack,size))
			resIt = it;

	//if not found create and add
	if (resIt == vec.end())
	{
		SimpleCallStack * newEntry = new SimpleCallStack(callStack,size);
		vec.push_back(newEntry);
		return *newEntry;
	}else {
		return **resIt;
	}
}

/*******************  FUNCTION  *********************/
std::ostream& operator<<(std::ostream& out, const SimpleTracer& tracer)
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
void SimpleTracer::resolveSymbols(FuncNameDic& dic) const
{
	for (SimpleBacktraceVectorMap::const_iterator itMap = callmaps.begin() ; itMap != callmaps.end() ; ++itMap)
	{
		const SimpleBacktraceVector & vec = itMap->second;
		for (SimpleBacktraceVector::const_iterator it = vec.begin() ; it != vec.end() ; ++it)
			(*it)->resolveSymbols(dic);
	}
}

/*******************  FUNCTION  *********************/
void typeToJson(htopml::JsonState& json, std::ostream& stream, const SimpleTracer& value)
{
	FuncNameDic dic;
	value.resolveSymbols(dic);

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
	json.closeStruct();
}

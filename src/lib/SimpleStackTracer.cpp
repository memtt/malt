/********************  HEADERS  *********************/
#include <cassert>
#include <cstdlib>
#include "SimpleStackTracer.h"

/*******************  FUNCTION  *********************/
SimpleStackTracer::SimpleStackTracer(void )
{

}

/*******************  FUNCTION  *********************/
SimpleStackTracer::~SimpleStackTracer(void )
{

}

/*******************  FUNCTION  *********************/
SimpleCallStackNode& SimpleStackTracer::getBacktraceInfo(void** callStack, int size)
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
		if ((*it)->getCallStack().equal(callStack,size))
			resIt = it;

	//if not found create and add
	if (resIt == vec.end())
	{
		SimpleCallStackNode * newEntry = new SimpleCallStackNode(callStack,size);
		vec.push_back(newEntry);
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
void typeToJson(htopml::JsonState& json, std::ostream& stream, const SimpleStackTracer& value)
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

/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//std
#include <cstdio>
//htopml
#include <json/JsonState.h>
//common
#include <common/Debug.hpp>
#include <common/CodeTiming.hpp>
//internals
#include "StackSTLMapList.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*******************  FUNCTION  *********************/
StackSTLMapListAbstract::StackSTLMapListAbstract(void )
{

}

/*******************  FUNCTION  *********************/
StackSTLMapListAbstract::~StackSTLMapListAbstract(void )
{
	if (!map.empty())
		MATT_ERROR("Caution, StackSTLMap didn't call celar so it might create a memory leak.");
}

/*******************  FUNCTION  *********************/
void StackSTLMapListAbstract::clear(void )
{
	for (InternalMap::iterator itMap = map.begin() ; itMap != map.end() ; ++itMap)
	{
		InternalVector & vect = itMap->second;
		for (InternalVector::iterator itVect = vect.begin() ; itVect != vect.end() ; ++itVect)
		{
			itVect->first->~Stack();
			MATT_FREE((void*)itVect->first);
			deleteObject(itVect->second);
		}
	}
	map.clear();
}

/*******************  FUNCTION  *********************/
StackSTLMapListAbstract::Node StackSTLMapListAbstract::getNode(const Stack& stack, int skipDepth)
{
	assert(stack.isValid());
	
	//calc current hash
	StackHash hash;
	CODE_TIMING("stackHash",hash = stack.hash(skipDepth));

	//search in current vector
	static CodeTiming tmap("searchInMap");
	ticks start = tmap.start();
	StackSTLMapListAbstract::InternalVector & vec = map[hash];
	tmap.end(start);

	//loop in vector to find the good one
	static CodeTiming tvec("searchInSubVect");
	start = tvec.start();
	StackSTLMapListAbstract::InternalVector::iterator resIt = vec.end();
	for (StackSTLMapListAbstract::InternalVector::iterator it = vec.begin() ; it != vec.end() ; ++it)
		if (Stack::partialCompare(*(it->first),0,stack,skipDepth))
			resIt = it;
	tvec.end(start);

	//if not found create and add
	if (resIt == vec.end())
	{
		StackSTLMapListAbstract::Node newEntry(copyStack(stack,skipDepth),this->allocateObject());
		CODE_TIMING("insertInVec",vec.push_back(newEntry));
		if (vec.size() > 100)
			fprintf(stderr,"warning get too much multiple in map sub-array, it may hust performances : %lu\n",vec.size());
		return newEntry;
	}else {
		return *resIt;
	}
}

/*******************  FUNCTION  *********************/
Stack* StackSTLMapListAbstract::copyStack(const Stack& stack, int skipDepth)
{
	void * ptr = MATT_MALLOC(sizeof(Stack));
	return new(ptr) Stack(stack,skipDepth);
}

/*******************  FUNCTION  *********************/
void* StackSTLMapListAbstract::getValue(const Stack& stack, int skipDepth)
{
	return getNode(stack,skipDepth).second;
}

/*******************  FUNCTION  *********************/
void StackSTLMapListAbstract::resolveSymbols(SymbolResolver& symbolResolver)
{
	for (InternalMap::iterator itMap = map.begin() ; itMap != map.end() ; ++itMap)
	{
		InternalVector & vect = itMap->second;
		for (InternalVector::iterator itVect = vect.begin() ; itVect != vect.end() ; ++itVect)
			itVect->first->resolveSymbols(symbolResolver);
	}
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const StackSTLMapListAbstract& value)
{
	json.openStruct();
	json.openFieldArray("stats");
	for (StackSTLMapListAbstract::InternalMap::const_iterator itMap = value.map.begin() ; itMap != value.map.end() ; ++itMap)
	{
		const StackSTLMapListAbstract::InternalVector & vect = itMap->second;
		for (StackSTLMapListAbstract::InternalVector::const_iterator itVect = vect.begin() ; itVect != vect.end() ; ++itVect)
		{
			json.printListSeparator();
			value.printJsonValue(json,itVect->first,itVect->second);
		}
	}
	json.closeFieldArray("stats");
	json.closeStruct();
}

}

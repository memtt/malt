/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <cassert>
#include <cstdio>
#include <json/ConvertToJson.h>
#include "SegmentTracker.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*******************  FUNCTION  *********************/
SegmentInfo::SegmentInfo(void)
{
	this->size = 0;
	this->callStack = NULL;
	this->allocTime = getticks();
}

/*******************  FUNCTION  *********************/
SegmentInfo::~SegmentInfo(void)
{

}

/*******************  FUNCTION  *********************/
SegmentTracker::SegmentTracker(void)
{
	
}

/*******************  FUNCTION  *********************/
SegmentTracker::~SegmentTracker(void)
{

}

/*******************  FUNCTION  *********************/
SegmentInfo* SegmentTracker::add(void* ptr, size_t size, SimpleCallStackNode* callStack)
{
	//check errors
	assert(this->get(ptr) == NULL);
	assert(callStack != NULL);
	//assert(size > 0);

	//create entry
	SegmentInfo & res = map[ptr];
	
	//setup content
	res.callStack = callStack;
	res.size = size;

	//ok return
	return &res;
}

/*******************  FUNCTION  *********************/
SegmentInfo* SegmentTracker::get(void* ptr)
{
	SegmentInfoMap::iterator it = map.find(ptr);
	if (it == map.end())
		return NULL;
	else
		return &it->second;
}

/*******************  FUNCTION  *********************/
void SegmentTracker::remove(void* ptr)
{
	map.erase(ptr);
}

/*******************  FUNCTION  *********************/
ticks SegmentInfo::getLifetime(void ) const
{
	return getticks() - allocTime;
}

/*******************  FUNCTION  *********************/
void SegmentTracker::fillLeaks(LeakInfoMap& leakMap) const
{
	for(SegmentInfoMap::const_iterator it = map.begin() ; it != map.end() ; ++it)
	{
		LeakInfo & info = leakMap[it->second.callStack];
		info.cnt++;
		info.mem+=it->second.size;
	}
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const SegmentTracker& value)
{
	LeakInfoMap leaks;
	value.fillLeaks(leaks);
	convertToJson(json,leaks);
}

/*******************  FUNCTION  *********************/
LeakInfo::LeakInfo(void)
{
	mem = 0;
	cnt = 0;
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const LeakInfoMap::const_iterator& it)
{
	json.openStruct();
	json.printField("stack",it->first->getCallStack());
	json.printField("count",it->second.cnt);
	json.printField("memory",it->second.mem);
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const LeakInfoMap& value)
{
	json.openArray();
	for (LeakInfoMap::const_iterator it = value.begin() ; it != value.end() ; ++it)
		json.printValue(it);
	json.closeArray();
}

}

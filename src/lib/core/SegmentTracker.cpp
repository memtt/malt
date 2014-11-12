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
SegmentInfo* SegmentTracker::add(void* ptr, size_t size, MATT::MMCallStackNode callStack)
{
	//check errors
	assert(this->get(ptr) == NULL);
	assert(callStack.valid());
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
		LeakInfo & info = leakMap[it->second.callStack.stack];
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
	json.printField("stack",*(it->first));
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

/*******************  FUNCTION  *********************/
void SegmentTracker::munmap(void* ptr, size_t size)
{
	//search for strict equal
	SegmentInfo * res = get(ptr);
	if (res != NULL && res->size == size)
	{
		remove(ptr);
		return;
	}
	
	//loop on all segments to check overlap
	for (SegmentInfoMap::iterator it = map.begin() ; it != map.end() ; ++it)
	{
		//calc start end
		size_t start = (size_t)it->first;
		size_t end = start + it->second.size;
		
		//check if partial overlap
		if ((size_t)ptr < end && (size_t)ptr+size >= start)
			split(it,ptr,size);
	}
}

/*******************  FUNCTION  *********************/
void SegmentTracker::split(SegmentInfoMap::iterator it, void* ptr, size_t size)
{
	//copy info
	size_t segStart = (size_t)it->first;
	SegmentInfo info = it->second;
	
	//compute left part if has one, update old or remove old
	if (ptr > it->first)
	{
		size_t start = (size_t)it->first;
		size_t end = (size_t)ptr;
		it->second.size = end-start;
	} else {
		map.erase(it);
	}
	
	//compute right part if has one
	if ((size_t)ptr + size < segStart+info.size)
	{
		size_t start = (size_t)ptr+size;
		size_t end = segStart + info.size;
		info.size = end - start;
		map[(void*)start] = info;
	}
}

}

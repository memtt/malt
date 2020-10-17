/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <cassert>
#include <cstdio>
#include <json/ConvertToJson.h>
#include "SegmentTracker.hpp"

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*******************  FUNCTION  *********************/
/**
 * Constructor of the segment info object to init values.
**/
SegmentInfo::SegmentInfo(void)
{
	this->size = 0;
	this->allocTime = getticks();
}

/*******************  FUNCTION  *********************/
/**
 * Destructor of segment info.
**/
SegmentInfo::~SegmentInfo(void)
{

}

/*******************  FUNCTION  *********************/
/**
 * Calculate the lifetime of the given segment comparing its allocation time
 * to the current one.
 * @return Return the lifetime in CPU cycles.
**/
ticks SegmentInfo::getLifetime(void ) const
{
	return getticks() - allocTime;
}

/*******************  FUNCTION  *********************/
/**
 * Constructor of segment tracker
**/
SegmentTracker::SegmentTracker(void)
{
	
}

/*******************  FUNCTION  *********************/
/**
 * Destructor of segment tracker.
**/
SegmentTracker::~SegmentTracker(void)
{

}

/*******************  FUNCTION  *********************/
/**
 * Add a segment to be tracked.
 * @param ptr Base address of the segment (returned by malloc).
 * @param size Size of the segment (argument of malloc).
 * @param callStack The call stack from where the allocation has been done.
 * @return Return a pointer to the segment info.
**/
SegmentInfo* SegmentTracker::add(void* ptr, size_t size, MALT::MMCallStackNode callStack)
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
/**
 * Search in the tracked segment the one defined by its address.
 * @param ptr Adress of the segment to search.
 * @return Pointer to the segment informations.
**/
SegmentInfo* SegmentTracker::get(void* ptr)
{
	SegmentInfoMap::iterator it = map.find(ptr);
	if (it == map.end())
		return NULL;
	else
		return &it->second;
}

/*******************  FUNCTION  *********************/
/**
 * Remove the segment identified by its address.
 * @param ptr Base address of the segment to remove.
**/
void SegmentTracker::remove(void* ptr)
{
	map.erase(ptr);
}

/*******************  FUNCTION  *********************/
/**
 * At the end of the execution, before dumping the allocation profile we can
 * loop on the tracked segment to get all the leaks.
 * @param leakMap Reference to the leak object to be used for profile dumping.
**/
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
/**
 * Convert the current object to json format.
 * @param json The json state to conver the current object.
 * @param value Reference to the object to convert to json.
**/
void convertToJson(htopml::JsonState& json, const SegmentTracker& value)
{
	LeakInfoMap leaks;
	value.fillLeaks(leaks);
	convertToJson(json,leaks);
}

/*******************  FUNCTION  *********************/
/**
 * Initilize the leak info object.
**/
LeakInfo::LeakInfo(void)
{
	mem = 0;
	cnt = 0;
}

/*******************  FUNCTION  *********************/
/**
 * Conver a leak info object to json.
 * @param json The json state object to make the conversion.
 * @param it Reference to the object to convert.
**/
void convertToJson(htopml::JsonState& json, const LeakInfoMap::const_iterator& it)
{
	json.openStruct();
	json.printField("stack",*(it->first));
	json.printField("count",it->second.cnt);
	json.printField("memory",it->second.mem);
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
/**
 * Convert the leak info map to json to be exported in the profile file.
 * @param json Define the json state object to use for the conversion.
 * @param value Reference to the map to convert.
**/
void convertToJson(htopml::JsonState& json, const LeakInfoMap& value)
{
	json.openArray();
	for (LeakInfoMap::const_iterator it = value.begin() ; it != value.end() ; ++it)
		json.printValue(it);
	json.closeArray();
}

/*******************  FUNCTION  *********************/
/**
 * Consider a munmap operation on the registered segement. If coverd by the
 * munmap operation the segment will be removed. If munmap parially cover
 * it we split it and keep the non unmapped part.
 * @param ptr Base address of the munmap operation.
 * @param size Size of the munmap operation.
**/
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
/**
 * If an munmap operation partialy copver a segment we need to split it
 * the keep only the non unmapped part in the registry.
 * @param it Iterator pointing the segment.
 * @param ptr Base address of the munmap operation
 * @param size Size of the munmap operation.
**/
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

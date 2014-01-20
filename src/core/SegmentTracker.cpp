/********************  HEADERS  *********************/
#include <cassert>
#include "SegmentTracker.hpp"

/*******************  NAMESPACE  ********************/
namespace ATT
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
	assert(size > 0);

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

}

#ifndef SEGMENT_FOLLOWER_H
#define SEGMENT_FOLLOWER_H

/********************  HEADERS  *********************/
#include <map>
#include "SimpleCallStackNode.h"

/*********************  CLASS  **********************/
struct SegmentInfo
{
	SegmentInfo(void);
	~SegmentInfo(void);
	SimpleCallStackNode * callStack;
	size_t size;
};

/*********************  TYPES  **********************/
typedef std::map<void*,SegmentInfo> SegmentInfoMap;

/*********************  CLASS  **********************/
class SegmentTracker
{
	public:
		SegmentTracker(void);
		~SegmentTracker(void);
		SegmentInfo * add(void* ptr, size_t size, SimpleCallStackNode* callStack);
		SegmentInfo * get(void * ptr);
		void remove(void * ptr);
	private:
		SegmentInfoMap map;
};

#endif //SEGMENT_FOLLOWER_H

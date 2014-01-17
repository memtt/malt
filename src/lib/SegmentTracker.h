#ifndef SEGMENT_FOLLOWER_H
#define SEGMENT_FOLLOWER_H

/********************  HEADERS  *********************/
#include <map>
#include "CallStackInfo.h"

/*********************  CLASS  **********************/
class SegmentInfo
{
	public:
		SegmentInfo(CallStackInfo * callStack);
		~SegmentInfo(void);
		CallStackInfo * getCallStack(void);
	private:
		CallStackInfo * callStack;
};

/*********************  TYPES  **********************/
typedef std::map<void*,SegmentInfo> SegmentInfoMap;

/*********************  CLASS  **********************/
class SegmentTracker
{
	public:
		SegmentTracker(void);
		~SegmentTracker(void);
		SegmentInfo * add(void * ptr,size_t size,CallStackInfo * callStack);
		SegmentInfo * get(void * ptr);
		void remove(void * ptr);
	private:
		SegmentInfoMap map;
};

#endif //SEGMENT_FOLLOWER_H

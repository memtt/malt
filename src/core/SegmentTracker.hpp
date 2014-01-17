#ifndef ATT_SEGMENT_FOLLOWER_HPP
#define ATT_SEGMENT_FOLLOWER_HPP

/********************  HEADERS  *********************/
#include <map>
#include "SimpleCallStackNode.hpp"

/*******************  NAMESPACE  ********************/
namespace ATT
{

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

}

#endif //ATT_SEGMENT_FOLLOWER_HPP

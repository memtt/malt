/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_SEGMENT_TRACKER_HPP
#define MATT_SEGMENT_TRACKER_HPP

/********************  HEADERS  *********************/
#include <map>
#include <cycle.h>
#include "SimpleCallStackNode.hpp"
#include <common/STLInternalAllocator.hpp>

/*******************  NAMESPACE  ********************/
namespace MATT
{

/********************  STRUCT  **********************/
struct LeakInfo
{
	LeakInfo(void);
	size_t mem;
	size_t cnt;
};

/*********************  CLASS  **********************/
struct SegmentInfo
{
	//funcs
	SegmentInfo(void);
	~SegmentInfo(void);
	ticks getLifetime(void) const;

	//vars
	SimpleCallStackNode * callStack;
	size_t size;
	ticks allocTime;
};

/*********************  TYPES  **********************/
typedef std::map<void*,SegmentInfo,std::less<void*>,STLInternalAllocator<std::pair<void*,SegmentInfo> > > SegmentInfoMap;
// typedef std::map<void*,SegmentInfo> SegmentInfoMap;
typedef std::map<const SimpleCallStackNode*,LeakInfo> LeakInfoMap;

/*********************  CLASS  **********************/
class SegmentTracker
{
	public:
		SegmentTracker(void);
		~SegmentTracker(void);
		SegmentInfo * add(void* ptr, size_t size, SimpleCallStackNode* callStack);
		SegmentInfo * get(void * ptr);
		void remove(void * ptr);
	public:
		friend void convertToJson(htopml::JsonState & json,const SegmentTracker & value);
	private:
		void fillLeaks(LeakInfoMap & leakMap) const;
	private:
		SegmentInfoMap map;
};

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const LeakInfoMap& value);

}

#endif //MATT_SEGMENT_TRACKER_HPP

/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.5
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/core/SegmentTracker.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2024
***********************************************************/

#ifndef MALT_SEGMENT_TRACKER_HPP
#define MALT_SEGMENT_TRACKER_HPP

/**********************************************************/
//STD
#include <map>
//from fftw
#include <cycle.h>
//internals
#include <common/STLInternalAllocator.hpp>
#include "SimpleCallStackNode.hpp"


/**********************************************************/
namespace MALT
{

/**********************************************************/
/**
 * @brief Short description of final memory leaks.
**/
struct LeakInfo
{
	LeakInfo(void);
	/** Total memory allocated. **/
	size_t mem;
	/** Number of allocated segments. **/
	size_t cnt;
};

/**********************************************************/
/**
 * @brief description of allocated segment to track them until deallocation time.
**/
struct SegmentInfo
{
	//funcs
	SegmentInfo(void);
	~SegmentInfo(void);
	ticks getLifetime(void) const;

	//vars
	/** Reference to the allocation call stack. **/
	MMCallStackNode callStack;
	/** Requested size of the allocated segment. **/
	size_t size;
	/** Timestamp of allocation. **/
	ticks allocTime;
};

/**********************************************************/
/** Map used to store informations about allocated segments. **/
typedef std::map<void*,SegmentInfo,std::less<void*>,STLInternalAllocator<std::pair<void*,SegmentInfo> > > SegmentInfoMap;
// typedef std::map<void*,SegmentInfo> SegmentInfoMap;
/** Map used to summurize the memory leaks at exit time. **/
typedef std::map<const Stack *,LeakInfo> LeakInfoMap;

/**********************************************************/
/**
 * Class used to track the allocated segments during their lifetime. It permit to attach
 * extra informations about them and to detect memory leaks at exit (segments still registered into the tracker).
 * 
 * @brief Class used to track allocated segments to store informations and detect memory leaks.
**/
class SegmentTracker
{
	public:
		SegmentTracker(void);
		~SegmentTracker(void);
		SegmentInfo * add(void* ptr, size_t size, MMCallStackNode callStack);
		SegmentInfo * get(void * ptr);
		void remove(void * ptr);
		void munmap(void * ptr,size_t size);
	public:
		friend void convertToJson(htopml::JsonState & json,const SegmentTracker & value);
	private:
		void fillLeaks(LeakInfoMap & leakMap) const;
		void split(SegmentInfoMap::iterator it, void * ptr, size_t size);
	private:
		/** Map used to store the allocated segment informations. **/
		SegmentInfoMap map;
};

/**********************************************************/
void convertToJson(htopml::JsonState& json, const LeakInfoMap& value);

}

#endif //MALT_SEGMENT_TRACKER_HPP

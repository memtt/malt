/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_CHUNK_TRACKER_HPP
#define MATT_CHUNK_TRACKER_HPP

/********************  HEADERS  *********************/
#include <cycle.h>
#include <portability/Spinlock.hpp>
#include <stacks/StackTree.hpp>
#include <allocators/STLInternalAllocator.hpp>
#include "Clock.hpp"
#include <cstdlib>
#include <map>

/********************  NAMESPACE  *******************/
namespace MATT
{

/*********************  STRUCT  *********************/
struct UserSegment
{
	size_t size;
	ticks birth;
	StackId stackId;
	StackTreeDataHandler dataHandler;
	int allocThreadId;
};

/*********************  STRUCT  *********************/
struct LeakSummary
{
	int count;
	int mem;
};

/*********************  TYPES  **********************/
typedef std::map<void*,UserSegment,std::less<void*>,STLInternalAllocator<std::pair<void*,UserSegment> > > UserSegmentInfoMap;
typedef std::map<StackId,LeakSummary,std::less<StackId>,STLInternalAllocator<std::pair<StackId,LeakSummary> > > LeakSummaryMap;

/*******************  FUNCTION  *********************/
void convertToJson( htopml::JsonState& json, const MATT::UserSegment& value );
void convertToJson( htopml::JsonState& json, const MATT::LeakSummary& value );

/*********************  CLASS  **********************/
class UserSegmentTracker
{
	public:
		UserSegmentTracker(bool threadSafe = true);
		void registerChunk( void* ptr, size_t size, ticks birth, MATT::StackId stackId, int allocThreadId, StackTreeDataHandler dataHandler );
		UserSegment unregister( Clock& clock, void* ptr );
		void buildLeakSummary(LeakSummaryMap & leakSummaryMap);
		friend void convertToJson( htopml::JsonState& json, const UserSegmentTracker& value );
	private:
		bool threadSafe;
		Spinlock lock;
		UserSegmentInfoMap map;
};

};

#endif //MATT_CHUNK_TRACKER_HPP

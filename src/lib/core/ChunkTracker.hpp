/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_CALL_COUNTER_HPP
#define MATT_CALL_COUNTER_HPP

/********************  HEADERS  *********************/
#include <cycle.h>
#include <cstdlib>
#include <map>

/********************  NAMESPACE  *******************/
namespace MATT
{

/*********************  STRUCT  *********************/
struct Chunk
{
	void * ptr;
	size_t size;
	ticks birth;
	int allocThreadId;
};

/*********************  CLASS  **********************/
typedef std::map<void*,SegmentInfo,std::less<void*>,STLInternalAllocator<std::pair<void*,SegmentInfo> > > SegmentInfoMap;

};

#endif //MATT_CALL_COUNTER_HPP

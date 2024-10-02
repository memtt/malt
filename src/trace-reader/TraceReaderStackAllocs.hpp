/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.3
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/trace-reader/TraceReaderStackAllocs.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

#ifndef MALT_TRACE_READER_STACK_ALLOCS_HPP
#define MALT_TRACE_READER_STACK_ALLOCS_HPP

/**********************************************************/
//STL C++
#include <map>
//internal
#include "TraceReader.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
struct AtTimeInfo
{
	AtTimeInfo(void) {this->count = 0;this->size = 0;};
	size_t count;
	size_t size;
};

/**********************************************************/
class TraceReaderStackAllocs : public TraceReader
{
	public:
		TraceReaderStackAllocs(Filter * filter = NULL);
		virtual void onStart(void);
		virtual void onData(MALT::AllocTracerChunk & chunk);
		virtual void onEnd(void);
	private:
		std::map<const MALT::Stack *,AtTimeInfo> map;
};

}

#endif //MALT_TRACE_READER_STACK_ALLOCS_HPP

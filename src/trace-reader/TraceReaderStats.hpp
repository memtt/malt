/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 08/2016
*    LICENSE  : CeCILL-C
*    FILE     : src/trace-reader/TraceReaderStats.hpp
*-----------------------------------------------------------
*    AUTHOR   : Mehdi Raza Jaffery (CERN) - 2016
***********************************************************/

#ifndef MALT_TRACE_READER_STATS_HPP
#define MALT_TRACE_READER_STATS_HPP

/**********************************************************/
#include <map>
#include "TraceReader.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
class Stack;

struct StackChunkStats
{
	const Stack * stackId;
	size_t count;
	size_t totalSize;
	size_t minSize;
	size_t maxSize;
};

typedef std::map<const Stack *, StackChunkStats> TraceReaderStatsMap;

/**********************************************************/
class TraceReaderStats : public TraceReader
{
	public:
		TraceReaderStats(Filter * filter = NULL);
		virtual void onStart(void);
		virtual void onData(MALT::AllocTracerChunk & chunk);
		virtual void onEnd(void);
	private:
		bool first;
		TraceReaderStatsMap stats;
};

}

#endif //MALT_TRACE_READER_STATS_HPP

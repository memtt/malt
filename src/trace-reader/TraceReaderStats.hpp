/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Jaffery, Mehdi Raza
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_TRACE_READER_STATS_HPP
#define MALT_TRACE_READER_STATS_HPP

/********************  HEADERS  *********************/
#include <map>
#include "TraceReader.hpp"

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*********************  TYPES  **********************/
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

/*********************  CLASS  **********************/
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

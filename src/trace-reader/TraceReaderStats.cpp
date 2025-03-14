/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 09/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/trace-reader/TraceReaderStats.cpp
*-----------------------------------------------------------
*    AUTHOR   : Mehdi Raza Jaffery (CERN) - 2016
*    AUTHOR   : Sébastien Valat - 2020 - 2024
***********************************************************/

/**********************************************************/
//STD
#include <cstdio>
//malt internals
#include "TraceReaderStats.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
TraceReaderStats::TraceReaderStats(Filter * filter): TraceReader(filter)
{
}

/**********************************************************/
void TraceReaderStats::onStart(void)
{
	this->first = true;
}

/**********************************************************/
void TraceReaderStats::onEnd(void)
{
	// We are organize the data like this:
	// [ [stackId, chunksCount, chunksTotalSize, chunksMinSize, chunksMaxSize], ... ]
	
	printf("[\n");

	for (TraceReaderStatsMap::const_iterator it = stats.begin() ; it != stats.end() ; ++it) {
		StackChunkStats item = it->second;
		printf("\t[\"%p\", %zu, %zu, %zu, %zu]", (void*)item.stackId, item.count, item.totalSize, item.minSize, item.maxSize);

		TraceReaderStatsMap::const_iterator final_iter = stats.end();
		--final_iter;
		if (it != final_iter) {
			printf(",\n");
		} else {
			printf("\n");
		}
	}
	
	printf("]\n");
}

/**********************************************************/
void TraceReaderStats::onData(MALT::AllocTracerChunk& chunk)
{
	TraceReaderStatsMap::iterator it = stats.find(chunk.allocStack);
	if(it == stats.end()) {
		StackChunkStats stackData;
		stackData.stackId = chunk.allocStack;
		stackData.count = 1;
		stackData.totalSize = chunk.size;
		stackData.minSize = chunk.size;
		stackData.maxSize = chunk.size;
		stats[chunk.allocStack] = stackData;
	} else {
		it->second.count++;
		it->second.totalSize += chunk.size;
		it->second.minSize = (it->second.minSize > chunk.size) ? chunk.size : it->second.minSize;
		it->second.maxSize = (it->second.maxSize < chunk.size) ? chunk.size : it->second.maxSize;
	}
}

}

/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Jaffery, Mehdi Raza
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//STD
#include <cstdio>
//malt internals
#include "TraceReaderStats.hpp"

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*******************  FUNCTION  *********************/
TraceReaderStats::TraceReaderStats(Filter * filter): TraceReader(filter)
{
}

/*******************  FUNCTION  *********************/
void TraceReaderStats::onStart(void)
{
	this->first = true;
}

/*******************  FUNCTION  *********************/
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

/*******************  FUNCTION  *********************/
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

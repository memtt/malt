/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 03/2020
*    LICENSE  : CeCILL-C
*    FILE     : src/trace-reader/TraceReaderStackAllocs.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2020
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

/**********************************************************/
//internal
#include "TraceReaderStackAllocs.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
TraceReaderStackAllocs::TraceReaderStackAllocs(Filter * filter): TraceReader(filter)
{
}

/**********************************************************/
void TraceReaderStackAllocs::onStart(void)
{
	this->map.clear();
}

/**********************************************************/
void TraceReaderStackAllocs::onEnd(void)
{
	bool first = true;
	printf("{");
	for (std::map<const MALT::Stack *,AtTimeInfo>::const_iterator it = map.begin() ; it != map.end() ; ++it)
	{
		if (!first)
			printf(",");
		else
			first = false;
		printf("\n\t\"%p\": [%zu,%zu]",(void*)it->first,it->second.count,it->second.size);
	}
	printf("\n}\n");
}

/**********************************************************/
void TraceReaderStackAllocs::onData(MALT::AllocTracerChunk& chunk)
{
	AtTimeInfo & info = map[chunk.allocStack];
	info.count++;
	info.size+=chunk.size;
	//printf("cur = %llu , %p => OK\n",chunk.timestamp,chunk.allocStack);
}

}

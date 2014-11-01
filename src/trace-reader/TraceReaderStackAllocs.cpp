/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//internal
#include "TraceReaderStackAllocs.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*******************  FUNCTION  *********************/
TraceReaderStackAllocs::TraceReaderStackAllocs(Filter * filter): TraceReader(filter)
{
}

/*******************  FUNCTION  *********************/
void TraceReaderStackAllocs::onStart(void)
{
	this->map.clear();
}

/*******************  FUNCTION  *********************/
void TraceReaderStackAllocs::onEnd(void)
{
	bool first = true;
	printf("{");
	for (std::map<const MATT::Stack *,AtTimeInfo>::const_iterator it = map.begin() ; it != map.end() ; ++it)
	{
		if (!first)
			printf(",");
		else
			first = false;
		printf("\n\t\"%p\": [%lu,%lu]",it->first,it->second.count,it->second.size);
	}
	printf("\n}\n");
}

/*******************  FUNCTION  *********************/
void TraceReaderStackAllocs::onData(MATT::AllocTracerChunk& chunk)
{
	AtTimeInfo & info = map[chunk.allocStack];
	info.count++;
	info.size+=chunk.size;
	//printf("cur = %llu , %p => OK\n",chunk.timestamp,chunk.allocStack);
}

}

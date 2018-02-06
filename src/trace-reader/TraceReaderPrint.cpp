/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.0.0
             DATE     : 02/2018
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//STD
#include <cstdio>
//malt internals
#include "TraceReaderPrint.hpp"

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*******************  FUNCTION  *********************/
TraceReaderPrint::TraceReaderPrint(Filter * filter): TraceReader(filter)
{
}

/*******************  FUNCTION  *********************/
void TraceReaderPrint::onStart(void)
{
	//open json array
	printf("[\n\t");
	this->first = true;
}

/*******************  FUNCTION  *********************/
void TraceReaderPrint::onEnd(void)
{
	//closejson array
	printf("\n]\n");
}

/*******************  FUNCTION  *********************/
void TraceReaderPrint::onData(MALT::AllocTracerChunk& chunk)
{
	if (!first)
		printf(",\n\t");
	//printf("{\n\t\tallocStack: %p\n\t\tsize: %lu\n\t\ttimestamp: %llu\n\t\tlifetime: %llu\n\t}",buffer[i].allocStack,buffer[i].size,buffer[i].timestamp,buffer[i].lifetime);
	printf("[\"%p\",\"%p\",%lu,%llu,%llu]",chunk.addr,(void*)chunk.allocStack,chunk.size,chunk.allocTime,chunk.lifetime);
	first = false;
}

}

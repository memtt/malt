/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
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
	printf("[\"%p\",\"%p\",%zu,%llu,%llu]",chunk.addr,(void*)chunk.allocStack,chunk.size,chunk.allocTime,chunk.lifetime);
	first = false;
}

}

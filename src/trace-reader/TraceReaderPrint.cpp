/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 03/2020
*    LICENSE  : CeCILL-C
*    FILE     : src/trace-reader/TraceReaderPrint.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2020
*    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
***********************************************************/

/**********************************************************/
//STD
#include <cstdio>
//malt internals
#include "TraceReaderPrint.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
TraceReaderPrint::TraceReaderPrint(Filter * filter): TraceReader(filter)
{
}

/**********************************************************/
void TraceReaderPrint::onStart(void)
{
	//open json array
	printf("[\n\t");
	this->first = true;
}

/**********************************************************/
void TraceReaderPrint::onEnd(void)
{
	//closejson array
	printf("\n]\n");
}

/**********************************************************/
void TraceReaderPrint::onData(MALT::AllocTracerChunk& chunk)
{
	if (!first)
		printf(",\n\t");
	printf("[\"%p\",\"%p\",%zu,%llu,%llu]",chunk.addr,(void*)chunk.allocStack,chunk.size,chunk.allocTime,chunk.lifetime);
	first = false;
}

}

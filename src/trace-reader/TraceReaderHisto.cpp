/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.5
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/trace-reader/TraceReaderHisto.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
***********************************************************/

/**********************************************************/
//STD
#include <cstdio>
//malt internals
#include "TraceReaderHisto.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
TraceReaderHisto::TraceReaderHisto(HistoCriteria histCrit, Filter * filter): TraceReader(filter)
{
	this->histCrit = histCrit;
}

/**********************************************************/
void TraceReaderHisto::onStart(void)
{
	this->histo.clear();
}

/**********************************************************/
void TraceReaderHisto::onEnd(void)
{
	//open json array
	printf("{\n");
	
	//print
	for (TraceReaderHistoMap::const_iterator it = histo.begin() ; it != histo.end() ; ++it)
		printf("	\"%llu\":%zu,\n",it->first,it->second);
	
	//closejson array
	printf("}\n");
}

/**********************************************************/
void TraceReaderHisto::onData(MALT::AllocTracerChunk& chunk)
{
	ticks indice;

	switch(histCrit)
	{
		case HISTO_SIZE:
			indice = chunk.size;
			break;
		case HISTO_LIFE:
			indice = chunk.lifetime;
			break;
		case HISTO_TIME:
			indice = chunk.allocTime;
			break;
	}
	
	TraceReaderHistoMap::iterator it = histo.find(indice);
	if (it == histo.end())
		histo[indice] = 1;
	else
		it->second++;
}

}

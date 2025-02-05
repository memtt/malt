/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.5
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/trace-reader/TraceReaderHisto.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

#ifndef MALT_TRACE_READER_HISTO_HPP
#define MALT_TRACE_READER_HISTO_HPP

/**********************************************************/
//STL C++
#include <map>
//internal
#include "TraceReader.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
enum HistoCriteria
{
	HISTO_SIZE,
	HISTO_LIFE,
	HISTO_TIME
};

/**********************************************************/
typedef std::map<ticks,size_t> TraceReaderHistoMap;

/**********************************************************/
class TraceReaderHisto : public TraceReader
{
	public:
		TraceReaderHisto(HistoCriteria histCrit,Filter * filter = NULL);
		virtual void onStart(void);
		virtual void onData(MALT::AllocTracerChunk & chunk);
		virtual void onEnd(void);
	private:
		HistoCriteria histCrit;
		TraceReaderHistoMap histo;
};

}

#endif //MALT_TRACE_READER_HISTO_HPP

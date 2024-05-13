/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.2
*    DATE     : 06/2023
*    LICENSE  : CeCILL-C
*    FILE     : src/trace-reader/TraceReaderHisto.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/
/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_TRACE_READER_HISTO_HPP
#define MALT_TRACE_READER_HISTO_HPP

/********************  HEADERS  *********************/
//STL C++
#include <map>
//internal
#include "TraceReader.hpp"

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*********************  ENUM  ***********************/
enum HistoCriteria
{
	HISTO_SIZE,
	HISTO_LIFE,
	HISTO_TIME
};

/*********************  TYPES  **********************/
typedef std::map<ticks,size_t> TraceReaderHistoMap;

/*********************  CLASS  **********************/
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

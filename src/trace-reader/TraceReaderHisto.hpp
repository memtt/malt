/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_TRACE_READER_HISTO_HPP
#define MATT_TRACE_READER_HISTO_HPP

/********************  HEADERS  *********************/
//STL C++
#include <map>
//internal
#include "TraceReader.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
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
		virtual void onData(MATT::AllocTracerChunk & chunk);
		virtual void onEnd(void);
	private:
		HistoCriteria histCrit;
		TraceReaderHistoMap histo;
};

}

#endif //MATT_TRACE_READER_HISTO_HPP

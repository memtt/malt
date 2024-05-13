/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.2
*    DATE     : 06/2023
*    LICENSE  : CeCILL-C
*    FILE     : src/trace-reader/TraceReader.hpp
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

#ifndef MALT_TRACE_READER_HPP
#define MALT_TRACE_READER_HPP

/********************  HEADERS  *********************/
//malt internals
#include <core/AllocTraceFile.hpp>

/*******************  NAMESPACE  ********************/
namespace MALT
{

/********************  STRUCT  **********************/
struct Filter
{
	Filter();
	bool accept (MALT::AllocTracerChunk & chunk);
	MALT::Stack * filterStack;
	size_t filterSize;
	ticks at;
};

/*********************  CLASS  **********************/
class TraceReader
{
	public:
		TraceReader(Filter * filter = NULL);
		virtual ~TraceReader(void);
		bool run(const char * filepath);
		void run(FILE * fp);
	protected:
		virtual void onStart(void) {};
		virtual void onData(MALT::AllocTracerChunk & chunk){};
		virtual void onEnd(void) {};
		Filter * filter;
};

}

#endif //MALT_TRACE_READER_HPP

/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/trace-reader/TraceReader.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
***********************************************************/

#ifndef MALT_TRACE_READER_HPP
#define MALT_TRACE_READER_HPP

/**********************************************************/
//malt internals
#include <core/AllocTraceFile.hpp>

/**********************************************************/
namespace MALT
{

/**********************************************************/
struct Filter
{
	Filter();
	bool accept (MALT::AllocTracerChunk & chunk);
	MALT::Stack * filterStack;
	size_t filterSize;
	ticks at;
};

/**********************************************************/
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

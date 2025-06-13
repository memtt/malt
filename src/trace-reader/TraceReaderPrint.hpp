/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.0
*    DATE     : 11/2014
*    LICENSE  : CeCILL-C
*    FILE     : src/trace-reader/TraceReaderPrint.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

#ifndef MALT_TRACE_READER_PRINT_HPP
#define MALT_TRACE_READER_PRINT_HPP

/**********************************************************/
#include "TraceReader.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
class TraceReaderPrint : public TraceReader
{
	public:
		TraceReaderPrint(Filter * filter = NULL);
		virtual void onStart(void);
		virtual void onData(MALT::AllocTracerChunk & chunk);
		virtual void onEnd(void);
	private:
		bool first;
};

}

#endif //MALT_TRACE_READER_PRINT_HPP

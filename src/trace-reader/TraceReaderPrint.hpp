/*****************************************************
             PROJECT  : MALT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_TRACE_READER_PRINT_HPP
#define MALT_TRACE_READER_PRINT_HPP

/********************  HEADERS  *********************/
#include "TraceReader.hpp"

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*********************  CLASS  **********************/
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

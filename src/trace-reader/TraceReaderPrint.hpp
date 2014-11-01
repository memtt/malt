/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_TRACE_READER_PRINT_HPP
#define MATT_TRACE_READER_PRINT_HPP

/********************  HEADERS  *********************/
#include "TraceReader.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  CLASS  **********************/
class TraceReaderPrint : public TraceReader
{
	public:
		TraceReaderPrint(Filter * filter = NULL);
		virtual void onStart(void);
		virtual void onData(MATT::AllocTracerChunk & chunk);
		virtual void onEnd(void);
	private:
		bool first;
};

}

#endif //MATT_TRACE_READER_PRINT_HPP

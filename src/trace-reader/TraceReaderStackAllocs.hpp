/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_TRACE_READER_STACK_ALLOCS_HPP
#define MATT_TRACE_READER_STACK_ALLOCS_HPP

/********************  HEADERS  *********************/
//STL C++
#include <map>
//internal
#include "TraceReader.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/********************  STRUCT  **********************/
struct AtTimeInfo
{
	AtTimeInfo(void) {this->count = 0;this->size = 0;};
	size_t count;
	size_t size;
};

/*********************  CLASS  **********************/
class TraceReaderStackAllocs : public TraceReader
{
	public:
		TraceReaderStackAllocs(Filter * filter = NULL);
		virtual void onStart(void);
		virtual void onData(MATT::AllocTracerChunk & chunk);
		virtual void onEnd(void);
	private:
		std::map<size_t,AtTimeInfo> map;
};

}

#endif //MATT_TRACE_READER_STACK_ALLOCS_HPP

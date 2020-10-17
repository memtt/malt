/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_TRACE_READER_STACK_ALLOCS_HPP
#define MALT_TRACE_READER_STACK_ALLOCS_HPP

/********************  HEADERS  *********************/
//STL C++
#include <map>
//internal
#include "TraceReader.hpp"

/*******************  NAMESPACE  ********************/
namespace MALT
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
		virtual void onData(MALT::AllocTracerChunk & chunk);
		virtual void onEnd(void);
	private:
		std::map<const MALT::Stack *,AtTimeInfo> map;
};

}

#endif //MALT_TRACE_READER_STACK_ALLOCS_HPP

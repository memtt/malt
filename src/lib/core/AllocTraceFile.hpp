 /*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_ALLOC_TRACER_HPP
#define MATT_ALLOC_TRACER_HPP

/*******************  FUNCTION  *********************/
//standard
#include <cstdio>
#include <cstdlib>
#include <cycle.h>
#include <string>

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  TYPES  **********************/
class Stack;
	
/********************  MACROS  **********************/
#define MATT_ALLOC_TRACER_BUFFER_SIZE 4096

/********************  STRUCT  **********************/
struct AllocTracerChunk
{
	const Stack * allocStack;
	size_t size;
	ticks timestamp;
	ticks lifetime;
};

/*********************  CLASS  **********************/
class AllocTraceFile
{
	public:
		AllocTraceFile(const std::string & file = "",size_t bufferSize = MATT_ALLOC_TRACER_BUFFER_SIZE);
		~AllocTraceFile(void);
		void open(const std::string & file);
		void close(void);
		void traceChunk(const Stack * allocStack,const Stack * freeStack,size_t size,ticks timestamp,ticks lifetime);
	private:
		void flush(void);
	private:
		FILE * fp;
		size_t pos;
		size_t bufferSize;
		AllocTracerChunk * buffer;
};

}

#endif //ATT_CALL_STACK_INFO_HPP

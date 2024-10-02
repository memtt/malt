/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.3
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/core/AllocTraceFile.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/
 
#ifndef MALT_ALLOC_TRACER_HPP
#define MALT_ALLOC_TRACER_HPP

/**********************************************************/
//standard
#include <cstdio>
#include <cstdlib>
#include <cycle.h>
#include <string>

/**********************************************************/
namespace MALT
{

/**********************************************************/
class Stack;
	
/**********************************************************/
/**
 * Buffer size to accumulate the element to dump to trace file to not
 * write them one by one.
**/
#define MALT_ALLOC_TRACER_BUFFER_SIZE 4096

/**********************************************************/
/**
 * Define an event to dump in binary format in the trace file.
 * @todo Ideally use uint64_t for sizes to be more strict in the output file, for
 * example to be usable from arch32 when writted on arch64.
 * 
 * @brief Structure dumped to allocation trace files.
**/
struct AllocTracerChunk
{
	/** ID of the call stack, by default we consider its memory address which is uniq. **/
	const Stack * allocStack;
	/** base address **/
	void * addr;
	/** Size of the allocated chunk (requested size) **/
	size_t size;
	/** Timestamp of creation in ticks. **/
	ticks allocTime;
	/** Lifetime of the chunk in ticks. **/
	ticks lifetime;
};

/**********************************************************/
/**
 * @brief Class to manage the generation of allocation trace file.
**/
class AllocTraceFile
{
	public:
		AllocTraceFile(const std::string & file = "",size_t bufferSize = MALT_ALLOC_TRACER_BUFFER_SIZE);
		~AllocTraceFile(void);
		void open(const std::string & file);
		void close(void);
		void traceChunk(const Stack * allocStack,const Stack * freeStack,void * addr,size_t size,ticks timestamp,ticks lifetime);
	private:
		void flush(void);
	private:
		/** Descriptor of the current output file. **/
		FILE * fp;
		/** Current position in the buffer, increase on traceChunk(). **/
		size_t pos;
		/** Size of the current buffer. Buffer is flushed when pos==size.**/
		size_t bufferSize;
		/** Pointer to the intenral buffer to accumulate before writing.**/
		AllocTracerChunk * buffer;
};

}

#endif //ATT_CALL_STACK_INFO_HPP

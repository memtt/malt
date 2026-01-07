/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 10/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/core/AllocTraceFile.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/
 
#ifndef MALT_ALLOC_TRACER_HPP
#define MALT_ALLOC_TRACER_HPP

/**********************************************************/
//standard
#include <cstdio>
#include <cstdlib>
#include <cycle.h>
#include <string>
#include "AllocTraceFormat.hpp"
#include "portability/Mutex.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
/**
 * Buffer size to accumulate the element to dump to trace file to not
 * write them one by one.
**/
#define MALT_ALLOC_TRACER_BUFFER_SIZE 4096

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
		void pushEvent(const AllocTracerEvent & event);
		void rename(const std::string & newFname);
	private:
		void flush(void);
	private:
		/** Remember the filename to rename at the end (python) */
		std::string fname{""};
		/** Descriptor of the current output file. **/
		FILE * fp{nullptr};
		/** Current position in the buffer, increase on traceChunk(). **/
		volatile size_t pos{0};
		/** Size of the current buffer. Buffer is flushed when pos==size.**/
		size_t bufferSize{MALT_ALLOC_TRACER_BUFFER_SIZE};
		size_t writingBufferSize{MALT_ALLOC_TRACER_BUFFER_SIZE * 128};
		/** Pointer to the intenral buffer to accumulate before writing.**/
		AllocTracerEvent * buffer{nullptr};
		/** Writing buffer */
		char * writingBuffer{nullptr};
		/** Mutex */
		Mutex mutex;
};

}

#endif //ATT_CALL_STACK_INFO_HPP

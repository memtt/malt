/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <cstring>
#include <cassert>
#include <common/Debug.hpp>
#include "AllocTraceFile.hpp"

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*******************  FUNCTION  *********************/
/**
 * Constructor of the trace file manager. It mostly setup the local parameters
 * and open the trace file.
 * @param file Define the path to the output file to use.
 * @param bufferSize define the size of the buffer to use.
**/
AllocTraceFile::AllocTraceFile(const std::string& file,size_t bufferSize)
{
	this->bufferSize = bufferSize;
	this->buffer = new AllocTracerChunk[bufferSize];
	
	if ( ! file.empty() )
		this->open(file);
}

/*******************  FUNCTION  *********************/
/**
 * Destructor of the trace file manager, it mostly close the current trace file
 * and delete the local buffer.
**/
AllocTraceFile::~AllocTraceFile(void)
{
	this->close();
	delete [] buffer;
}

/*******************  FUNCTION  *********************/
/**
 * Function used to open and prepare the trace file.
 * @param file Define the file path to use.
**/
void AllocTraceFile::open(const std::string& file)
{
	this->close();
	
	//open
	this->fp = fopen(file.c_str(),"w");
	
	//check error
	if (this->fp == NULL)
		MALT_ERROR_ARG("Failed to open file %1 : %2\n").arg(file).argStrErrno();
	
	//reset pos in buffer
	this->pos = 0;
}

/*******************  FUNCTION  *********************/
/**
 * Close the trace file but take care of flushing the current buffer before
 * doing it.
**/
void AllocTraceFile::close(void)
{
	//nothing to do
	if (fp == NULL)
		return;
	
	this->flush();
	fclose(fp);
	this->fp = NULL;
}

/*******************  FUNCTION  *********************/
/**
 * Add a trace event for the given memory chunk. It can generate a write() to the file
 * if the buffer is full.
 * @param allocStack Define the call stack responsible of the allocation of the current chunk.
 * @param freeStack Define the call stack responsible of the deallocation of the current chunk.
 * @param size Define the requested size of the chuink at allocation time.
 * @param timestamp Define the allocation timestamp.
 * @param lifetime Define the lifetime of the current chunk.
**/
void AllocTraceFile::traceChunk(const MALT::Stack* allocStack, const MALT::Stack* freeStack, void* addr, size_t size, ticks timestamp, ticks lifetime)
{
	//check errors
	assert(pos < bufferSize);
	
	//write value
	AllocTracerChunk & entry = buffer[pos];
	entry.allocStack = allocStack;
	entry.lifetime = lifetime;
	entry.size = size;
	entry.addr = addr;
	entry.allocTime = timestamp;
	
	//inc pos
	this->pos++;

	//need flush
	if (pos == bufferSize)
		this->flush();
}

/*******************  FUNCTION  *********************/
/**
 * Function responsible to flush the buffer content to the file and reset
 * the internal counter.
**/
void AllocTraceFile::flush(void)
{
	//check
	assert(pos <= bufferSize);
	
	//no file
	if (this->fp == NULL)
		return;
	
	//write
	size_t size = fwrite(buffer,sizeof(buffer[0]),pos,fp);
	assumeArg(size == pos,"Failed to write all datas with fwrite, check for interuption, need a loop here for some thread context. %1 != %2").arg(size).arg(pos).end();
	
	//reset pos
	pos = 0;
}

}

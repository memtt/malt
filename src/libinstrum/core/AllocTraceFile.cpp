/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2022
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/core/AllocTraceFile.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014 - 2022
***********************************************************/

/**********************************************************/
#include <cstring>
#include <cassert>
#include <common/Debug.hpp>
#include "AllocTraceFile.hpp"
#include "common/TakeLock.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
/**
 * Constructor of the trace file manager. It mostly setup the local parameters
 * and open the trace file.
 * @param file Define the path to the output file to use.
 * @param bufferSize define the size of the buffer to use.
**/
AllocTraceFile::AllocTraceFile(const std::string& file,size_t bufferSize)
{
	this->bufferSize = bufferSize;
	this->buffer = new AllocTracerEvent[bufferSize];
	this->writingBuffer = new char[bufferSize*1024];
	this->pos = 0;
	this->fp = NULL;
	if ( ! file.empty() )
		this->open(file);
}

/**********************************************************/
/**
 * Destructor of the trace file manager, it mostly close the current trace file
 * and delete the local buffer.
**/
AllocTraceFile::~AllocTraceFile(void)
{
	this->close();
	delete [] buffer;
	delete [] writingBuffer;
}

/**********************************************************/
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

	//remember
	this->fname = file;
}

/**********************************************************/
void AllocTraceFile::rename(const std::string & newFname)
{
	//nothing to do
	if (this->fname == newFname)
		return;

	//rename
	::rename(this->fname.c_str(), newFname.c_str());
	this->fname = newFname;
}

/**********************************************************/
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

/**********************************************************/
void AllocTraceFile::pushEvent(const AllocTracerEvent & event)
{
	//skip NOP
	if (event.type == EVENT_NOP)
		return;

	MALT_START_CRITICAL(this->mutex);
		//check errors
		assert(this->pos < this->bufferSize);

		//set
		this->buffer[pos] = event;

		//inc pos
		this->pos++;

		//need flush
		if (pos == bufferSize)
			this->flush();
	MALT_END_CRITICAL;
}

/**********************************************************/
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

	//convert
	size_t cur = 0;
	for (size_t i = 0 ; i < pos ; i++) {
		AllocTracerEvent & event = this->buffer[i];
		cur += sprintf(this->writingBuffer + cur, "%x\t%zx\t%zx\t%llx\t%llx\t%zx\t%zx\t%zx\t%zx\n", event.type, event.threadId, (size_t)event.callStack, event.time, event.cost, (size_t)event.addr, event.size, event.extra.generic.extra1, event.extra.generic.extra2);
	}
	
	//write
	size_t size = fwrite(this->writingBuffer,1,cur,fp);
	assumeArg(size == cur,"Failed to write all datas with fwrite, check for interuption, need a loop here for some thread context. %1 != %2").arg(size).arg(pos).end();
	
	//reset pos
	pos = 0;
}

}

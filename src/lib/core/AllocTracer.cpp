/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <cstring>
#include <cassert>
#include <common/Debug.hpp>
#include "AllocTracer.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*******************  FUNCTION  *********************/
AllocTracer::AllocTracer(const std::string& file,size_t bufferSize)
{
	this->bufferSize = bufferSize;
	this->buffer = new AllocTracerChunk[bufferSize];
	
	if ( ! file.empty() )
		this->open(file);
}

/*******************  FUNCTION  *********************/
AllocTracer::~AllocTracer(void)
{
	this->close();
	delete [] buffer;
}

/*******************  FUNCTION  *********************/
void AllocTracer::open(const std::string& file)
{
	this->close();
	
	//open
	this->fp = fopen(file.c_str(),"w");
	
	//check error
	if (this->fp == NULL)
		MATT_ERROR_ARG("Failed to open file %1 : %2\n").arg(file).argStrErrno();
	
	//reset pos in buffer
	this->pos = 0;
}

/*******************  FUNCTION  *********************/
void AllocTracer::close(void)
{
	//nothing to do
	if (fp == NULL)
		return;
	
	this->flush();
	fclose(fp);
	this->fp = NULL;
}

/*******************  FUNCTION  *********************/
void AllocTracer::traceChunk(const Stack* allocStack, const Stack* freeStack, size_t size, ticks timestamp, ticks lifetime)
{
	//check errors
	assert(pos < bufferSize);
	
	//write value
	AllocTracerChunk & entry = buffer[pos];
	entry.allocStack = allocStack;
	entry.lifetime = lifetime;
	entry.size = size;
	entry.timestamp = timestamp;
	
	//inc pos
	this->pos++;

	//need flush
	if (pos == bufferSize)
		this->flush();
}

/*******************  FUNCTION  *********************/
void AllocTracer::flush(void)
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

/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 03/2015
*    LICENSE  : CeCILL-C
*    FILE     : src/trace-reader/TraceReader.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014
*    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
***********************************************************/

/**********************************************************/
//malt internals
#include "TraceReader.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
#define BUFFER_SIZE 1024

/**********************************************************/
Filter::Filter()
{
	this->at = 0;
	this->filterSize = 0;
	this->filterStack = NULL;
}

/**********************************************************/
bool Filter::accept(MALT::AllocTracerChunk& chunk)
{
	return (filterSize == 0 || filterSize == chunk.size) 
	    && (filterStack == NULL || filterStack == chunk.allocStack)
		&& (at == 0 || (chunk.allocTime <= at && chunk.allocTime + chunk.lifetime >= at));
}



/**********************************************************/
TraceReader::TraceReader(Filter * filter)
{
	this->filter = filter;
}

/**********************************************************/
TraceReader::~TraceReader(void)
{

}

/**********************************************************/
bool TraceReader::run(const char* fname)
{
	//error
	if (fname == NULL)
	{
		fprintf(stderr,"Missing arguement, please provide the trace file as parameter.\n");
		return false;
	}
	
	//open the file
	FILE * fp = fopen(fname,"r");
	if (fp == NULL)
	{
		perror(fname);
		return false;
	}
	
	//run
	this->run(fp);
	
	//close
	fclose(fp);
	return true;
}

/**********************************************************/
void TraceReader::run(FILE* fp)
{
	//error
	if (fp == NULL)
		return;
	
	//handler
	this->onStart();
	
	//loop
	MALT::AllocTracerChunk buffer[BUFFER_SIZE];
	while (!feof(fp))
	{
		size_t size = fread(buffer,sizeof(buffer[0]),BUFFER_SIZE,fp);
		for (size_t i = 0 ; i < size ; i++)
		{
			bool acc = false;

			//check accept
			if (this->filter == NULL)
				acc = true;
			else
				acc = this->filter->accept(buffer[i]);

			//eat if accepted
			if (acc)
				this->onData(buffer[i]);
		}
	}
	
	//handler
	this->onEnd();
}

}

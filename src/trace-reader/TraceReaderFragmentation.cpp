/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//STD
#include <cstdio>
//malt internals
#include <common/Debug.hpp>
#include "TraceReaderFragmentation.hpp"

/*******************  NAMESPACE  ********************/
namespace MALT
{

/********************  MACROS  **********************/
#define PAGE_SIZE 4096
#define PAGE_MASK (~(PAGE_SIZE-1))

/*******************  FUNCTION  *********************/
TraceReaderFragmentation::TraceReaderFragmentation(ticks tStep1, ticks tStep2,bool details, MALT::Filter* filter): TraceReader(filter)
{
	if (tStep1 >= tStep2)
	{
		fprintf(stderr,"Invalid times, need to get first (%llu) smaller than the second (%llu) !",tStep1,tStep2);
		abort();
	}
	this->tStep1 = tStep1;
	this->tStep2 = tStep2;
	this->details = details;
}

/*******************  FUNCTION  *********************/
void TraceReaderFragmentation::onStart(void)
{
	this->chunks.clear();
}

/*******************  FUNCTION  *********************/
void TraceReaderFragmentation::onEnd(void)
{
	this->checkForSharedPages();
	if (this->details)
		this->printDetails();
	else
		this->printPerCallStack();
}

/*******************  FUNCTION  *********************/
void TraceReaderFragmentation::printPerCallStack(void)
{
	//list
	FragmentationChunkPerCallStackMap allocated;
	FragmentationChunkPerCallStackMap freed;
	
	//fill
	for (FragmentationChunkMap::iterator it = chunks.begin() ; it != chunks.end() ; ++it)
	{
		FragmentationChunk & info = it->second;
		if (info.haveSharedPage && info.presentAtStep2)
			allocated[info.chunk.allocStack].push(info.chunk.size,info.chunk.lifetime);
		else if (info.haveSharedPage && ! info.presentAtStep2)
			freed[info.chunk.allocStack].push(info.chunk.size,info.chunk.lifetime);
	}
	
	//print
	printf("{\n");
	printf("\t\"allocated\":[\n");
	
	for (FragmentationChunkPerCallStackMap::iterator it = allocated.begin() ; it != allocated.end() ; ++it)
		it->second.print(it->first);

	printf("\t],\n");
	printf("\t\"freed\":[\n");
	
	for (FragmentationChunkPerCallStackMap::iterator it = freed.begin() ; it != freed.end() ; ++it)
		it->second.print(it->first);
	
	printf("\t],\n");
	printf("}\n");
}

/*******************  FUNCTION  *********************/
void TraceReaderFragmentation::printDetails(void)
{
	printf("{\n");
	printf("\tallocated:[\n");
	for (FragmentationChunkMap::iterator it = chunks.begin() ; it != chunks.end() ; ++it)
	{
		FragmentationChunk & info = it->second;
		if (info.haveSharedPage && info.presentAtStep2)
			printf("\t\t{\"addr\":\"%p\",\"size\":\"%zu\",\"lifetime\":\"%llu\",\"stack\":\"%p\"},\n",info.chunk.addr,info.chunk.size,info.chunk.lifetime,(void*)info.chunk.allocStack);
	}
	printf("\t],\n");
	printf("\tfreed:[\n");
	for (FragmentationChunkMap::iterator it = chunks.begin() ; it != chunks.end() ; ++it)
	{
		FragmentationChunk & info = it->second;
		if (info.haveSharedPage && ! info.presentAtStep2)
			printf("\t\t{\"addr\":\"%p\",\"size\":\"%zu\",\"lifetime\":\"%llu\",\"stack\":\"%p\"},\n",info.chunk.addr,info.chunk.size,info.chunk.lifetime,(void*)info.chunk.allocStack);
	}
	printf("\t],\n");
	printf("}\n");
}

/*******************  FUNCTION  *********************/
void TraceReaderFragmentation::checkForSharedPages(void)
{
	for (FragmentationChunkMap::iterator it = chunks.begin() ; it != chunks.end() ; ++it)
	{
		for (FragmentationChunkMap::iterator it2 = chunks.begin() ; it2 != chunks.end() ; ++it2)
		{
			bool noSamePresent = (it->second.presentAtStep2 != it2->second.presentAtStep2);
			bool sharedPages   = (((size_t)it->first & PAGE_MASK) == ((size_t)it2->first + (it2->second.chunk.size & PAGE_MASK)))
			                    ||(((size_t)it2->first & PAGE_MASK) == ((size_t)it->first + (it->second.chunk.size & PAGE_MASK)));
			if (noSamePresent && sharedPages)
				it->second.haveSharedPage = it2->second.haveSharedPage = true;
		}
	}
}

/*******************  FUNCTION  *********************/
void TraceReaderFragmentation::onData(MALT::AllocTracerChunk& chunk)
{
	//is alive on step 1, then register in list
	if (chunk.allocTime <= tStep1 && chunk.allocTime+chunk.lifetime > tStep1)
	{
		FragmentationChunk & info = chunks[chunk.addr];
		info.chunk = chunk;
		info.presentAtStep2 = (chunk.allocTime+chunk.lifetime > tStep2);
		info.haveSharedPage = false;
	}
}

/*******************  FUNCTION  *********************/
FragmentationChunkPerCallStack::FragmentationChunkPerCallStack()
{
	this->count = 0;
	this->sum = 0;
	this->sumLifetime = 0;
	this->max = 0;
	this->min = -1;
}

/*******************  FUNCTION  *********************/
void FragmentationChunkPerCallStack::push(size_t size, ticks lifetime)
{
	this->sum+=size;
	this->count++;
	if (size > this->max)
		this->max = size;
	if (size < this->min)
		this->min = size;
	this->sumLifetime += lifetime;
}

/*******************  FUNCTION  *********************/
void FragmentationChunkPerCallStack::print(const Stack * stack) const
{
	printf("\t\t{\"stack\":\"%p\",\"count\":%zu,\"sum\":%zu,\"min\":%zu,\"max\":%zu,\"sumLifetime\":%llu},\n",
		   (void*)stack,this->count,this->sum,this->min,this->max,this->sumLifetime);
}

}

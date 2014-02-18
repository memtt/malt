/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include "SimpleAllocator.hpp"
#include <sys/mman.h>
#include <common/Debug.hpp>
#include "Helpers.hpp"
#include "CodeTiming.hpp"
#include <cstdio>
#include <iostream>

/*******************  NAMESPACE  ********************/
#define MATT_PAGE_SIZE 4096

/***************** USING NAMESPACE ******************/
using namespace std;

/*******************  NAMESPACE  ********************/
namespace MATT
{

/********************* GLOBALS **********************/
SimpleAllocator * gblInternaAlloc = NULL;

/*******************  FUNCTION  *********************/
SimpleAllocator::SimpleAllocator(bool threadSafe, size_t sysReqSize)
{
	this->sysReqSize = sysReqSize;
	this->threadSafe = threadSafe;
	this->cur = NULL;
	this->totalMemory = 0;
	this->unusedMemory = 0;
}

/*******************  FUNCTION  *********************/
void SimpleAllocator::requestSystemMemory(void)
{
	if (cur != NULL)
		freeList.insertNext(cur);
	cur = (Chunk*)mmap(NULL,sysReqSize,PROT_READ|PROT_WRITE,MAP_ANON|MAP_PRIVATE,0,0);
	assumeArg(cur != MAP_FAILED,"Failed to request memory with mmap for internal allocator : %s.").argStrErrno().end();
	cur->size = sysReqSize - sizeof(*cur);
	touchMemory(cur,cur->size);
	this->totalMemory += sysReqSize;
	this->unusedMemory += sysReqSize;
}

/*******************  FUNCTION  *********************/
void* SimpleAllocator::malloc(size_t size)
{
	assert(this != NULL);

	//timer
	CODE_TIMING_FUNC_START("internalMalloc");
		
	//nothing to do
	if (size == 0)
		return NULL;
	
	//round to minimal
	if (size <= MATT_ALLOC_MIN_SIZE)
		size = MATT_ALLOC_MIN_SIZE;
	
	//round to multiple of pointer size
	if (size % sizeof(MATT_ALLOC_BASIC_ALIGN) != 0)
		size += MATT_ALLOC_BASIC_ALIGN - (size % MATT_ALLOC_BASIC_ALIGN);
	assert(size % MATT_ALLOC_BASIC_ALIGN == 0);
	
	//search in list
	Chunk * chunk = NULL;
	MATT_OPTIONAL_CRITICAL(lock,threadSafe)
		chunk = getInList(size);

		//if empty, check with cur
		if (chunk == NULL)
			chunk = getInCur(size);
		
		if (chunk == NULL)
			chunk = getInSys(size);
		
		if (chunk != NULL)
		{
			assert(unusedMemory >= chunk->getTotalSize());
			this->unusedMemory -= chunk->getTotalSize();
		}
	MATT_END_CRITICAL
	
	//timer
	CODE_TIMING_FUNC_STOP("internalMalloc");

	//ok return
	return chunk->getBody();
}

/*******************  FUNCTION  *********************/
void SimpleAllocator::free(void* ptr)
{
	assert(this != NULL);
	
	//timer
	CODE_TIMING_FUNC_START("internalFree");

	//nothing to do
	if (ptr == NULL)
		return;
	
	//get chunk
	Chunk * chunk = Chunk::getFromBody(ptr);
	assert(chunk != NULL);
	if (chunk == NULL)
		return;
	
	//register in free list
	MATT_OPTIONAL_CRITICAL(lock,threadSafe)
		this->unusedMemory += chunk->getTotalSize();
		assert(unusedMemory <= totalMemory);
		freeList.insertNext(chunk);
	MATT_END_CRITICAL
	
	//timer
	CODE_TIMING_FUNC_STOP("internalFree");
}

/*******************  FUNCTION  *********************/
size_t Chunk::getTotalSize(void)
{
	return size + sizeof(*this);
}

/*******************  FUNCTION  *********************/
Chunk* SimpleAllocator::getInSys(size_t size)
{
	requestSystemMemory();
	return getInCur(size);
}

/*******************  FUNCTION  *********************/
Chunk* SimpleAllocator::getInCur(size_t size)
{
	//check
	if (cur == NULL)
		return NULL;
	if (cur->size < size)
		return NULL;
	
	//if need to split
	Chunk * res = cur;
	if (cur->size - size > MATT_ALLOC_SPLIT_THRESOLD)
	{
		Chunk * next = cur->split(size);
		cur = next;
	} else {
		cur = NULL;
	}

	return res;
}

/*******************  FUNCTION  *********************/
Chunk* SimpleAllocator::getInList(size_t size)
{
	FreeChunk * f = freeList.next;
	Chunk * res = NULL;
	
	while (f != &freeList && res == NULL)
	{
		if (f->chunk.canContain(size))
		{
			f->removeFromList();
			res = &f->chunk;
		}
		f = f->next;
	}
	
	if (res != NULL)
	{
		if (res->size - size > MATT_ALLOC_SPLIT_THRESOLD)
		{
			Chunk * next = res->split(size);
			freeList.insertNext(next);
		}
	}
	
	return res;
}

/*******************  FUNCTION  *********************/
Chunk::Chunk(size_t size)
{
	this->size = size;
}

/*******************  FUNCTION  *********************/
bool Chunk::canContain(size_t size)
{
	return this->size >= size;
}

/*******************  FUNCTION  *********************/
void* Chunk::getBody(void)
{
	if (this == NULL)
		return NULL;
	else
		return (void*)(this+1);
}

/*******************  FUNCTION  *********************/
Chunk* Chunk::getFromBody(void* ptr)
{
	if (ptr == NULL)
		return NULL;
	else
		return ((Chunk*)ptr)-1;
}

/*******************  FUNCTION  *********************/
Chunk* Chunk::split(size_t size)
{
	//check
	assert(this->size - size  > MATT_ALLOC_SPLIT_THRESOLD);
	assert(size % MATT_ALLOC_BASIC_ALIGN == 0);
	
	Chunk * next = (Chunk*)((size_t)(this+1)+size);
	next->size = this->size - size - sizeof(*this);
	
	this->size = size;

	return next;
}

/*******************  FUNCTION  *********************/
FreeChunk::FreeChunk(void)
{
	this->next = this;
	this->prev = this;
}

/*******************  FUNCTION  *********************/
void FreeChunk::insertNext(Chunk* chunk)
{
	FreeChunk * fchunk = (FreeChunk*)chunk;

	fchunk->next = this->next;
	fchunk->prev = this;
	this->next->prev = fchunk;
	this->next = fchunk;
}

/*******************  FUNCTION  *********************/
void FreeChunk::removeFromList(void)
{
	this->prev->next = next;
	this->next->prev = prev;
	this->prev = this;
	this->next = this;
}

/*******************  FUNCTION  *********************/
size_t SimpleAllocator::getTotalMemory(void)
{
	return totalMemory;
}

/*******************  FUNCTION  *********************/
size_t SimpleAllocator::getUnusedMemory(void)
{
	return unusedMemory;
}

/*******************  FUNCTION  *********************/
size_t SimpleAllocator::getMaxSize(void) const
{
	return sysReqSize;
}

/*******************  FUNCTION  *********************/
void SimpleAllocator::printState(void) const
{
	cerr << "=============================================================== MATT MEMORY =====================================================================" << endl;
	cerr << "Internal memory : allocated = ";
	Helpers::printValue(cerr,totalMemory,"o");
	cerr << " , unused = ";
	Helpers::printValue(cerr,unusedMemory,"o");
	cerr << endl;
	cerr << "=================================================================================================================================================" << endl;
}

/*******************  FUNCTION  *********************/
size_t SimpleAllocator::getInuseMemory(void)
{
	return totalMemory - unusedMemory;
}

/*******************  FUNCTION  *********************/
void SimpleAllocator::touchMemory(void* ptr, size_t size)
{
	if (ptr == NULL || ptr == MAP_FAILED)
		return;
	for (int i = 0 ; i < size ; i += MATT_PAGE_SIZE)
		*(char*)ptr = '\0';
}

};

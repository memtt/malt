/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//std
#include <cstdio>
#include <cstring>
#include <cassert>
#include <iostream>
//OS dependent (need to move)
#include <sys/mman.h>
//internal commons
#include "Debug.hpp"
#include "Helpers.hpp"
#include "CodeTiming.hpp"
//object to implement
#include "SimpleAllocator.hpp"
#include <portability/OS.hpp>

/*******************  NAMESPACE  ********************/
/**
 * Define the page size we consider.
**/
#define MALT_PAGE_SIZE 4096

/***************** USING NAMESPACE ******************/
using namespace std;

/*******************  NAMESPACE  ********************/
namespace MALT
{

/********************* GLOBALS **********************/
SimpleAllocator * gblInternaAlloc = NULL;

/*******************  FUNCTION  *********************/
/**
 * Constructor of the simple allocator.
 * @param threadSafe Enable of disable thread locking.
 * @param sysReqSize Define the size of memory segments we request to the OS with mmap when the allocator need more memory.
 * It must be a multiple of MALT_PAGE_SIZE.
**/
SimpleAllocator::SimpleAllocator(bool threadSafe, size_t sysReqSize)
{
	//check
	assert(sysReqSize % MALT_PAGE_SIZE == 0);
	
	//setup params
	this->sysReqSize = sysReqSize;
	this->threadSafe = threadSafe;
	
	//setup state
	this->cur = NULL;
	this->totalMemory = 0;
	this->unusedMemory = 0;
	this->curSearchInList = NULL;
}

/*******************  FUNCTION  *********************/
/**
 * Request more memory from the OS.
 * It will register the current segment into free list and point the new one allocated with
 * size.
 * 
 * The memory is guarty to be physically mapped so we can safely substract getTotalMemory()
 * from the RSS value we read from /proc/self/statm to get the application memory consumption
 * without considering our own one.
**/
void SimpleAllocator::requestSystemMemory(size_t size)
{
	//errors
	assert(size % MALT_PAGE_SIZE == 0);
	
	//register too small chunk if have one
	if (cur != NULL)
		freeList.insertNext(cur);
	
	//allocate new one
	cur = (Chunk*)mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_ANON|MAP_PRIVATE|MAP_POPULATE,0,0);
	assumeArg(cur != MAP_FAILED,"Failed to request memory with mmap for internal allocator : %s.").argStrErrno().end();
	
	//update chunk info
	cur->size = size - sizeof(*cur);

	//to be sure for all OS
	touchMemory(cur->getBody(),cur->size);
	
	//account memory
	this->totalMemory += size;
	this->unusedMemory += size;
}

/*******************  FUNCTION  *********************/
/**
 * Short implementation of malloc. It search in free list for available segments.
 * If not found, try to split the current segment. If too small, it request a new one from
 * the system.
 * 
 * This function is thread safe by using locks (mutex).
 *
 * @param size of the segment to allocate.
**/
void* SimpleAllocator::malloc(size_t size)
{
	assert(this != NULL);

	//timer
	CODE_TIMING_FUNC_START("internalMalloc");
		
	//nothing to do
	if (size == 0)
		return NULL;
	
	//round to minimal
	if (size < MALT_ALLOC_MIN_SIZE)
		size = MALT_ALLOC_MIN_SIZE;
	
	//round to multiple of pointer size
	if (size % sizeof(MALT_ALLOC_BASIC_ALIGN) != 0)
		size += MALT_ALLOC_BASIC_ALIGN - (size % MALT_ALLOC_BASIC_ALIGN);
	assert(size % MALT_ALLOC_BASIC_ALIGN == 0);
	
	//search in list
	Chunk * chunk = NULL;
	MALT_OPTIONAL_CRITICAL(lock,threadSafe)
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
	MALT_END_CRITICAL
	
	//timer
	CODE_TIMING_FUNC_STOP("internalMalloc");

	//ok return
	return chunk->getBody();
}

/*******************  FUNCTION  *********************/
/**
 * Free the given segment by registering it into the freelist.
 * In this simple allocator we do not do segment merging.
**/
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
	MALT_OPTIONAL_CRITICAL(lock,threadSafe)
		this->unusedMemory += chunk->getTotalSize();
		assert(unusedMemory <= totalMemory);
		freeList.insertNext(chunk);
		this->curSearchInList = NULL;
	MALT_END_CRITICAL
	
	//timer
	CODE_TIMING_FUNC_STOP("internalFree");
}

/*******************  FUNCTION  *********************/
/**
 * Try to get a segment from the system. It will automatically split it to the requested size.
 * @param size Requested segment size (body size without the extra header one).
**/
Chunk* SimpleAllocator::getInSys(size_t size)
{
	size_t reqSize = size + sizeof(Chunk);
	
	if (reqSize <= sysReqSize)
		requestSystemMemory(sysReqSize);
	else if (reqSize % MALT_PAGE_SIZE == 0)
		requestSystemMemory(reqSize);
	else
		requestSystemMemory(reqSize + (MALT_PAGE_SIZE - reqSize % MALT_PAGE_SIZE));
	return getInCur(size);
}

/*******************  FUNCTION  *********************/
/**
 * Try to get a segment from the current active one by splitting it.
 * If to small the function return NULL, so you need to manually fallback to requestSystemMemory().
 * @param size Requested segment size (body size without the extra header one).
**/
Chunk* SimpleAllocator::getInCur(size_t size)
{
	//check
	if (cur == NULL)
		return NULL;
	if (cur->size < size)
		return NULL;
	
	//if need to split
	Chunk * res = cur;
	if (cur->size - size > MALT_ALLOC_SPLIT_THRESOLD)
	{
		Chunk * next = cur->split(size);
		cur = next;
	} else {
		cur = NULL;
	}

	//check
	assert(res->size >= size);
	
	//ok return
	return res;
}

/*******************  FUNCTION  *********************/
/**
 * Try to find a valid segment in the free list. If not found
 * the function return NULL so you need to manually fallback onto getInCur().
 * It will not overallocate more than MALT_ALLOC_SPLIT_THRESOLD.
 * @param size Requested segment size (body size without the extra header one).
**/
Chunk* SimpleAllocator::getInList(size_t size)
{
	if (this->curSearchInList == NULL)
		this->curSearchInList = freeList.next;
	
	FreeChunk * f = this->curSearchInList;
	Chunk * res = NULL;
	int maxTest = 16;
	
	while (f != &freeList && res == NULL && maxTest >= 0)
	{
		if (f->chunk.canContain(size))
		{
			f->removeFromList();
			res = &f->chunk;
		}
		f = f->next;
		maxTest--;
	}
	
	if (res != NULL)
	{
		if (res->size - size > MALT_ALLOC_SPLIT_THRESOLD)
		{
			Chunk * next = res->split(size);
			freeList.insertNext(next);
		}
		this->curSearchInList = NULL;
	}
	
	return res;
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
/**
 * Print the current memory state of the allocator.
**/
void SimpleAllocator::printState(std::ostream & out) const
{
	out << "=============================================================== MALT MEMORY =====================================================================" << endl;
	out << "Internal memory : allocated = ";
	Helpers::printValue(out,totalMemory,"o");
	out << " , unused = ";
	Helpers::printValue(out,unusedMemory,"o");
	out << endl;
	out << "=================================================================================================================================================" << endl;
}

/*******************  FUNCTION  *********************/
/**
 * Return the total memory currently provided to the users of the allocator.
**/
size_t SimpleAllocator::getInuseMemory(void)
{
	return totalMemory - unusedMemory;
}

/*******************  FUNCTION  *********************/
/**
 * Touch the memory to ensure physical mapping. 
 * 
 * @TODO It's redundant with MAP_POPULATE, maybe remove.
**/
void SimpleAllocator::touchMemory(void* ptr, size_t size)
{
	if (ptr == NULL || ptr == MAP_FAILED)
		return;
	for (size_t i = 0 ; i < size ; i += MALT_PAGE_SIZE)
		*(char*)ptr = '\0';
}

/*******************  FUNCTION  *********************/
/**
 * Provide a short implement of realloc. It uses a basic method based on malloc/copy/free.
**/
void* SimpleAllocator::realloc(void * old,size_t size)
{
	void * res = NULL;
	
	//allocate if non null size
	if (size > 0)
		res = this->malloc(size);
	
	//copy and free the old segment
	if (old != NULL)
	{
		Chunk * chunk = Chunk::getFromBody(old);
		assert(chunk != NULL);
		size_t cpSize = min(chunk->size,size);
		memcpy(res,old,cpSize);
		this->free(old);
	}
	
	//return
	return res;
}

/*******************  FUNCTION  *********************/
/**
 * Construct a memory chunk by setting-up his header struct.
 * @param size Requested segment size (body size without the extra header one).
**/
Chunk::Chunk(size_t size)
{
	this->size = size;
}

/*******************  FUNCTION  *********************/
/**
 * @return Return the total memory size used by the segment (accounting the header structure size).
**/
size_t Chunk::getTotalSize(void)
{
	return size + sizeof(*this);
}

/*******************  FUNCTION  *********************/
/**
 * Check if the current chunk can contain at least size byte of data (body size).
**/
bool Chunk::canContain(size_t size)
{
	return this->size >= size;
}

/*******************  FUNCTION  *********************/
/**
 * Return the base address of the body part of the allocated segment.
**/
void* Chunk::getBody(void)
{
	//tmp copy to avoid recent compiler warning
	void * tmp = this;
	if (tmp == NULL)
		return NULL;
	else
		return (void*)(this+1);
}

/*******************  FUNCTION  *********************/
/**
 * Find the header address from the body address of an allocated chunk.
 * It return NULL if ptr is NULL.
 * 
 * @b CAUTION, there is currently no check if the given address is invalid.
**/
Chunk* Chunk::getFromBody(void* ptr)
{
	if (ptr == NULL)
		return NULL;
	else
		return ((Chunk*)ptr)-1;
}

/*******************  FUNCTION  *********************/
/**
 * Split the current segment in two subsegments and return the address of the second one.
 * @param size Cut the current segment the keep only size byte for the body part of the first segment.
**/
Chunk* Chunk::split(size_t size)
{
	//check
	assert(this->size - size  > MALT_ALLOC_SPLIT_THRESOLD);
	assert(size % MALT_ALLOC_BASIC_ALIGN == 0);
	
	//split
	Chunk * next = (Chunk*)((size_t)(this+1)+size);
	next->size = this->size - size - sizeof(*this);
	
	//check sum
	assert(sizeof(*this) + size + next->size == this->size);
	
	//update cur size
	this->size = size;

	//return
	return next;
}

/*******************  FUNCTION  *********************/
/**
 * Init a free chunk header. It mostly init the list pointers added to the default chunk header.
**/
FreeChunk::FreeChunk(void)
{
	this->next = this;
	this->prev = this;
}

/*******************  FUNCTION  *********************/
/**
 * Insert a new free chunk after the current one.
**/
void FreeChunk::insertNext(Chunk* chunk)
{
	//check
	assert(chunk->getTotalSize() >= sizeof(FreeChunk));
	
	//cast
	FreeChunk * fchunk = (FreeChunk*)chunk;

	//setup
	fchunk->next = this->next;
	fchunk->prev = this;
	
	//insert
	this->next->prev = fchunk;
	this->next = fchunk;
}

/*******************  FUNCTION  *********************/
/**
 * Remove the current segment from the list.
**/
void FreeChunk::removeFromList(void)
{
	//check
	MALT_ASSERT(this->prev->next == this);
	MALT_ASSERT(this->next->prev == this);
	
	//remove
	this->prev->next = next;
	this->next->prev = prev;
	
	//for security
	this->prev = this;
	this->next = this;
}

/*******************  FUNCTION  *********************/
void initInternalAlloc(bool threadSafe)
{
	if (gblInternaAlloc == NULL)
		gblInternaAlloc = new SimpleAllocator(threadSafe);
}

}

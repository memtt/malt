/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_SIMPLE_ALLOCATOR_HPP
#define MATT_SIMPLE_ALLOCATOR_HPP

/********************  HEADERS  *********************/
#include <cstdlib>
#include <portability/Spinlock.hpp>

/*******************  NAMESPACE  ********************/
namespace MATT
{

/********************  MACROS  **********************/
#define MATT_ALLOC_SYS_REQ_SIZE (256*1024)
#define MATT_ALLOC_MIN_SIZE (sizeof(FreeChunk) - sizeof(Chunk))
#define MATT_ALLOC_BASIC_ALIGN (sizeof(void*))
#define MATT_ALLOC_SPLIT_THRESOLD (2*MATT_ALLOC_MIN_SIZE)

/********************  STRUCT  **********************/
struct Chunk
{
	size_t size;
	
	Chunk(size_t size = 0);
	void * getBody(void);
	bool canContain(size_t size);
	Chunk * split(size_t size);
	static Chunk * getFromBody(void * ptr);
	size_t getTotalSize(void);
};

/********************  STRUCT  **********************/
struct FreeChunk
{
	FreeChunk(void);
	void insertNext(Chunk * chunk);
	void removeFromList(void);
	Chunk chunk;
	FreeChunk * prev;
	FreeChunk * next;
};

/*********************  CLASS  **********************/
class SimpleAllocator
{
	public:
		SimpleAllocator(bool threadSafe = false,size_t sysReqSize = MATT_ALLOC_SYS_REQ_SIZE);
		void * malloc(size_t size);
		void free(void * ptr);
		size_t getTotalMemory(void);
		size_t getUnusedMemory(void);
		size_t getInuseMemory(void);
		size_t getMaxSize(void) const;
		void printState(void) const;
	protected:
		void requestSystemMemory(void);
		void touchMemory(void * ptr,size_t size);
		Chunk * getInList(size_t size);
		Chunk * getInCur(size_t size);
		Chunk * getInSys(size_t size);
	private:
		FreeChunk freeList;
		Chunk * cur;
		size_t sysReqSize;
		size_t totalMemory;
		size_t unusedMemory;
		bool threadSafe;
		Spinlock lock;
};

/*********************  CLASS  **********************/
extern SimpleAllocator * gblInternaAlloc;

};

#endif //MATT_SIMPLE_ALLOCATOR_HPP

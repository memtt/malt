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
//standard
#include <cstdlib>
//portability dependent code
#include <portability/Spinlock.hpp>

/*******************  NAMESPACE  ********************/
namespace MATT
{

/********************  MACROS  **********************/
/** Default memory size to request to system when internal memory is full. **/
#define MATT_ALLOC_SYS_REQ_SIZE (256*1024)
/** 
 * Minimal size of allocated chunk (depend on header size for chunk and free chunk handler). 
 * Size is defined as usable size without takking accound of chunk header size.
**/
#define MATT_ALLOC_MIN_SIZE (sizeof(FreeChunk) - sizeof(Chunk))
/** Lower memory alignment considerd by allocator to split unused segements. **/
#define MATT_ALLOC_BASIC_ALIGN (sizeof(void*))
/** Do not split elements smaller than given size. **/
#define MATT_ALLOC_SPLIT_THRESOLD (2*MATT_ALLOC_MIN_SIZE)

/********************  MACROS  **********************/
/** Wrapper to replace malloc for internal allocations. **/
#define MATT_MALLOC(x) gblInternaAlloc->malloc(x)
/** Wrapper to replace free for internal allocations. **/
#define MATT_FREE(x) gblInternaAlloc->free(x)
/** Wrapper to replace realloc for internal allocations. **/
#define MATT_REALLOC(x,y) gblInternaAlloc->realloc((x),(y))

/********************  STRUCT  **********************/
/**
 * Define an allocated memory chunk. It's defined only by its size as we do not consider
 * segment merging for this simple allocator.
 * 
 * @brief Define allocated chunks for internal allocator.
**/
struct Chunk
{
	//functions
	Chunk(size_t size = 0);
	void * getBody(void);
	bool canContain(size_t size);
	Chunk * split(size_t size);
	static Chunk * getFromBody(void * ptr);
	size_t getTotalSize(void);
	
	//vars
	/** Internal size of the allocated chunk, so it will not take in account the size of the chunk structure. **/
	size_t size;
};

/********************  STRUCT  **********************/
/**
 * Define a free chunk which can be placed into a double linked list.
 * It inherit from Chunk structure.
 * 
 * @brief Define allocated chunks for internal allocator.
**/
struct FreeChunk
{
	//functions
	FreeChunk(void);
	void insertNext(Chunk * chunk);
	void removeFromList(void);
	
	//vars
	/** Inherit from chunk structure, CAUTION, must be the first entry of the struct. **/
	Chunk chunk;
	/** Pointer to the next free chunk of himself. **/
	FreeChunk * prev;
	/** Pointer to the previous free chunk of himself. **/
	FreeChunk * next;
};

/*********************  CLASS  **********************/
class SimpleAllocator
{
	public:
		SimpleAllocator(bool threadSafe = false,size_t sysReqSize = MATT_ALLOC_SYS_REQ_SIZE);
		void * malloc(size_t size);
		void free(void * ptr);
		void * realloc(void * old,size_t size);
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

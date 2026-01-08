/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/core/AllocTraceFormat.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
*    AUTHOR   : Sébastien Valat - 2026
***********************************************************/
 
#ifndef MALT_ALLOC_TRACE_FORMAT_HPP
#define MALT_ALLOC_TRACE_FORMAT_HPP

/**********************************************************/
//standard
#include <cstdio>
#include <cstdlib>
#include <cycle.h>
#include <string>
#include <common/Debug.hpp>

/**********************************************************/
namespace MALT
{

/**********************************************************/
class Stack;

/**********************************************************/
enum AllocTraceEventType
{
	//NOP
	EVENT_NOP               = 0x00,
	//C malloc
	EVENT_C_MALLOC          = 0x01,
	EVENT_C_CALLOC          = 0x02,
	EVENT_C_REALLOC         = 0x03,
	EVENT_C_FREE            = 0x04,
	EVENT_C_POSIX_MEMALIGN  = 0x05,
	EVENT_C_ALIGNED_ALLOC   = 0x06,
	EVENT_C_MEMALIGN        = 0x07,
	EVENT_C_VALLOC          = 0x08,
	EVENT_C_PVALLOC         = 0x09,
	//lifetime
	EVENT_C_REALLOC_LFTIME  = 0x10,
	//c mmap
	EVENT_C_MMAP            = 0x21,
	EVENT_C_MUNMAP          = 0x22,
	EVENT_C_MREMAP          = 0x23,
	//python obj
	EVENT_PY_OBJ_MALLOC     = 0x30,
	EVENT_PY_OBJ_FREE       = 0x31,
	EVENT_PY_OBJ_REALLOC    = 0x32,
	EVENT_PY_OBJ_CALLOC     = 0x33,
	//python mem
	EVENT_PY_MEM_MALLOC     = 0x40,
	EVENT_PY_MEM_FREE       = 0x41,
	EVENT_PY_MEM_REALLOC    = 0x42,
	EVENT_PY_MEM_CALLOC     = 0x43,
	//pyton raw
	EVENT_PY_RAW_MALLOC     = 0x50,
	EVENT_PY_RAW_FREE       = 0x51,
	EVENT_PY_RAW_REALLOC    = 0x52,
	EVENT_PY_RAW_CALLOC     = 0x53,
	//GPU MALLOC
	EVENT_GPU_MALLOC        = 0x60,
	EVENT_GPU_FREE          = 0x61,
};

/**********************************************************/
/**
 * Define an event to dump in binary format in the trace file.
 * 
 * @brief Structure dumped to allocation trace files.
**/
struct AllocTracerEvent
{
	inline size_t toCString(char * buffer, size_t bufferSize) const;
	inline bool fromCString(const char * buffer);
	inline bool fromFilePointer(FILE * fp);

	/** Define the event type */
	AllocTraceEventType type{EVENT_NOP};
	/** thread ID */
	size_t threadId{0};
	/** ID of the call stack, by default we consider its memory address which is uniq. **/
	const Stack * callStack{nullptr};
	/** Timestamp of creation in ticks. **/
	ticks time{0};
	/** Duration in ticks. **/
	ticks cost{0};
	/** base address **/
	void * addr{nullptr};
	/** Size of the allocated chunk (requested size) **/
	size_t size{0};
	/**extra */
	union {
		struct {
			void * oldAddr;
			size_t oldSize;
		} realloc;
		struct {
			void * newAddr;
			size_t newSize;
		} mremap;
		struct {
			size_t align;
		} memalign;
		struct {
			size_t lifetime;
			const Stack * allocStack;
		} free;
		struct {
			size_t lifetime;
			const Stack * allocStack;
		} reallocLifetime;
		struct {
			size_t extra1;
			size_t extra2;
		} generic;
	} extra;
};

/**********************************************************/
/**
 * Dump the struct under a textual line to be dumped in to a file.
 * @param buffer The textual buffer to fill.
 * @param bufferSize The size of the buffer not to overflow.
 */
size_t AllocTracerEvent::toCString(char * buffer, size_t bufferSize) const
{
	int status = snprintf(buffer, bufferSize, "%x\t%zx\t%zx\t%llx\t%llx\t%zx\t%zx\t%zx\t%zx\n",
		this->type,
		this->threadId, 
		(size_t)this->callStack, 
		this->time, 
		this->cost,
		(size_t)this->addr,
		this->size,
		this->extra.generic.extra1,
		this->extra.generic.extra2
	);
	assume(status >= 0, "Invalid status !");
	return status;
}

/**********************************************************/
/**
 * Load the content from the C string.
 * @param buffer The buffer to load from.
 */
bool AllocTracerEvent::fromCString(const char * buffer)
{
	unsigned int intType = EVENT_NOP;
	int cnt = sscanf(buffer, "%x\t%zx\t%zx\t%llx\t%llx\t%zx\t%zx\t%zx\t%zx\n",
		&intType,
		&this->threadId, 
		(size_t*)&this->callStack, 
		&this->time, 
		&this->cost,
		(size_t*)&this->addr,
		&this->size,
		&this->extra.generic.extra1,
		&this->extra.generic.extra2
	);
	if (cnt == 9) {
		this->type = (AllocTraceEventType)intType;
		return true;
	} else {
		return false;
	}
}

/**********************************************************/
/**
 * Load the content from the FILE pointer.
 * @param fp The file to load from.
 */
bool AllocTracerEvent::fromFilePointer(FILE * fp)
{
	unsigned int intType = EVENT_NOP;
	int cnt = fscanf(fp, "%x\t%zx\t%zx\t%llx\t%llx\t%zx\t%zx\t%zx\t%zx\n",
		&intType,
		&this->threadId, 
		(size_t*)&this->callStack, 
		&this->time, 
		&this->cost,
		(size_t*)&this->addr,
		&this->size,
		&this->extra.generic.extra1,
		&this->extra.generic.extra2
	);
	if (cnt == 9) {
		this->type = (AllocTraceEventType)intType;
		return true;
	} else {
		return false;
	}
}

}

#endif //MALT_ALLOC_TRACE_FORMAT_HPP

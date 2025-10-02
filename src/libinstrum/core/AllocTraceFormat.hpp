/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2014
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/core/AllocTraceFile.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014
***********************************************************/
 
#ifndef MALT_ALLOC_TRACE_FORMAT_HPP
#define MALT_ALLOC_TRACE_FORMAT_HPP

/**********************************************************/
//standard
#include <cstdio>
#include <cstdlib>
#include <cycle.h>
#include <string>

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
};

/**********************************************************/
/**
 * Define an event to dump in binary format in the trace file.
 * 
 * @brief Structure dumped to allocation trace files.
**/
struct AllocTracerEvent
{
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

}

#endif //MALT_ALLOC_TRACE_FORMAT_HPP

/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/core/AllocTraceFormat.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
*    AUTHOR   : Sébastien Valat - 2026
***********************************************************/
 
#ifndef MALT_TRACE_SPEC_H
#define MALT_TRACE_SPEC_H

/**********************************************************/
//standard
#include <stdio.h>
#include <stdlib.h>

/**********************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************/
typedef struct malt_stack_s malt_stack_t;

/**********************************************************/
enum malt_trace_event_type_e
{
	//NOP
	MALT_TRACE_EVENT_NOP               = 0x00,
	//C malloc
	MALT_TRACE_EVENT_C_MALLOC          = 0x01,
	MALT_TRACE_EVENT_C_CALLOC          = 0x02,
	MALT_TRACE_EVENT_C_REALLOC         = 0x03,
	MALT_TRACE_EVENT_C_FREE            = 0x04,
	MALT_TRACE_EVENT_C_POSIX_MEMALIGN  = 0x05,
	MALT_TRACE_EVENT_C_ALIGNED_ALLOC   = 0x06,
	MALT_TRACE_EVENT_C_MEMALIGN        = 0x07,
	MALT_TRACE_EVENT_C_VALLOC          = 0x08,
	MALT_TRACE_EVENT_C_PVALLOC         = 0x09,
	//lifetime
	MALT_TRACE_EVENT_C_REALLOC_LFTIME  = 0x10,
	//c mmap
	MALT_TRACE_EVENT_C_MMAP            = 0x21,
	MALT_TRACE_EVENT_C_MUNMAP          = 0x22,
	MALT_TRACE_EVENT_C_MREMAP          = 0x23,
	//python obj
	MALT_TRACE_EVENT_PY_OBJ_MALLOC     = 0x30,
	MALT_TRACE_EVENT_PY_OBJ_FREE       = 0x31,
	MALT_TRACE_EVENT_PY_OBJ_REALLOC    = 0x32,
	MALT_TRACE_EVENT_PY_OBJ_CALLOC     = 0x33,
	//python mem
	MALT_TRACE_EVENT_PY_MEM_MALLOC     = 0x40,
	MALT_TRACE_EVENT_PY_MEM_FREE       = 0x41,
	MALT_TRACE_EVENT_PY_MEM_REALLOC    = 0x42,
	MALT_TRACE_EVENT_PY_MEM_CALLOC     = 0x43,
	//pyton raw
	MALT_TRACE_EVENT_PY_RAW_MALLOC     = 0x50,
	MALT_TRACE_EVENT_PY_RAW_FREE       = 0x51,
	MALT_TRACE_EVENT_PY_RAW_REALLOC    = 0x52,
	MALT_TRACE_EVENT_PY_RAW_CALLOC     = 0x53,
};
typedef enum malt_trace_event_type_e malt_trace_event_type_t;

/**********************************************************/
/**
 * Define an event to dump in binary format in the trace file.
 * 
 * @brief Structure dumped to allocation trace files.
**/
struct malt_trace_event_s
{
	/** Define the event type */
	malt_trace_event_type_t type;
	/** thread ID */
	size_t threadId;
	/** ID of the call stack, by default we consider its memory address which is uniq. **/
	const malt_stack_t * callStack;
	/** Timestamp of creation in ticks. **/
	unsigned long long time;
	/** Duration in ticks. **/
	unsigned long long cost;
	/** base address **/
	void * addr;
	/** Size of the allocated chunk (requested size) **/
	size_t size;
	/**extra */
	union {
		struct {
			size_t nmemb;
		} calloc;
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
			const malt_stack_t * allocStack;
		} free;
		struct {
			size_t lifetime;
			const malt_stack_t * allocStack;
		} reallocLifetime;
		struct {
			size_t extra1;
			size_t extra2;
		} generic;
	} extra;
};
typedef struct malt_trace_event_s malt_trace_event_t;

/**********************************************************/
/**
 * Init a malt_trace_event_t struct with its default.
 * @param event Pointer to the trace event to init.
 */
void malt_trace_event_init(malt_trace_event_t * event);

/**
 * Dump the struct under a textual line to be dumped in to a file.
 * @param event Pointer to the trace event to init.
 * @param buffer The textual buffer to fill.
 * @param bufferSize The size of the buffer not to overflow.
 */
size_t malt_trace_event_to_string(const malt_trace_event_t * event, char * buffer, size_t buffer_size);

/**
 * Load the content from the C string.
 * @param event Pointer to the trace event to init.
 * @param buffer The buffer to load from.
 */
bool malt_trace_event_from_string(malt_trace_event_t * event, const char * buffer);

/**
 * Load the content from the FILE pointer.
 * @param event Pointer to the trace event to init.
 * @param fp The file to load from.
 */
bool malt_trace_event_from_fp(malt_trace_event_t * event, FILE * fp);

/**
 * Convert to C code under string format.
 * 
 */
size_t malt_trace_event_to_c_code(const malt_trace_event_t * event, char * buffer, size_t buffer_size);

#ifdef __cplusplus
}
#endif

#endif //MALT_TRACE_SPEC_H

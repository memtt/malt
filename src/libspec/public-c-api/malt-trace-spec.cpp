/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/core/AllocTraceFormat.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
*    AUTHOR   : Sébastien Valat - 2026
***********************************************************/

/**********************************************************/
//standard
#include <cassert>
#include <cstdlib>
//malt
#include "malt-trace-spec.h"

/**********************************************************/
void malt_trace_event_init(malt_trace_event_t * event)
{
	event->type      = MALT_TRACE_EVENT_NOP;
	event->threadId  =  0;
	event->callStack = NULL;
	event->time      = 0;
	event->cost      = 0;
	event->addr      = NULL;
	event->size      = 0;
}

/**********************************************************/
size_t malt_trace_event_to_string(const malt_trace_event_t * event, char * buffer, size_t buffer_size)
{
	//check
	assert(event != nullptr);
	assert(buffer != nullptr);

	//handle in case
	if (event == nullptr)
		return false;
	if (buffer == nullptr)
		return false;

	//convert
	int status = snprintf(buffer, buffer_size, "%x\t%zx\t%zx\t%llx\t%llx\t%zx\t%zx\t%zx\t%zx\n",
		event->type,
		event->threadId, 
		(size_t)event->callStack, 
		event->time, 
		event->cost,
		(size_t)event->addr,
		event->size,
		event->extra.generic.extra1,
		event->extra.generic.extra2
	);

	//error
	if (status < 0)
		return 0;

	//ok
	return status;
}

/**********************************************************/
bool malt_trace_event_from_string(malt_trace_event_t * event, const char * buffer)
{
	//check
	assert(event != nullptr);
	assert(buffer != nullptr);

	//handle in case
	if (event == nullptr)
		return false;
	if (buffer == nullptr)
		return false;

	//init
	malt_trace_event_init(event);

	//load
	unsigned int intType = MALT_TRACE_EVENT_NOP;
	int cnt = sscanf(buffer, "%x\t%zx\t%zx\t%llx\t%llx\t%zx\t%zx\t%zx\t%zx\n",
		&intType,
		&event->threadId, 
		(size_t*)&event->callStack, 
		&event->time, 
		&event->cost,
		(size_t*)&event->addr,
		&event->size,
		&event->extra.generic.extra1,
		&event->extra.generic.extra2
	);

	//check
	if (cnt == 9) {
		event->type = (malt_trace_event_type_t)intType;
		return true;
	} else {
		return false;
	}
}

/**********************************************************/
bool malt_trace_event_from_fp(malt_trace_event_t * event, FILE * fp)
{
	//check
	assert(event != nullptr);
	assert(fp != nullptr);

	//handle in case
	if (event == nullptr)
		return false;
	if (fp == nullptr)
		return false;

	//init
	malt_trace_event_init(event);

	//load
	unsigned int intType = MALT_TRACE_EVENT_NOP;
	int cnt = fscanf(fp, "%x\t%zx\t%zx\t%llx\t%llx\t%zx\t%zx\t%zx\t%zx\n",
		&intType,
		&event->threadId, 
		(size_t*)&event->callStack, 
		&event->time, 
		&event->cost,
		(size_t*)&event->addr,
		&event->size,
		&event->extra.generic.extra1,
		&event->extra.generic.extra2
	);

	//check status
	if (cnt == 9) {
		event->type = (malt_trace_event_type_t)intType;
		return true;
	} else {
		return false;
	}
}

/**********************************************************/
size_t malt_trace_event_to_c_code(const malt_trace_event_t * event, char * buffer, size_t buffer_size)
{
	//trivial
	if (event == nullptr || buffer == nullptr || buffer_size == 0)
		return 0;

	//head
	const size_t cnt = snprintf(buffer, buffer_size, "//AT=%llu, THREAD=%zu, COST=%llu\n", event->time, event->threadId, event->cost);
	buffer += cnt;
	buffer_size -= cnt;

	//cases
	switch (event->type)
	{
		//NOP
		case MALT_TRACE_EVENT_NOP:
			return snprintf(buffer, buffer_size, "/* NOP */\n");
		//C malloc
		case MALT_TRACE_EVENT_C_MALLOC:
			return snprintf(buffer, buffer_size, "ptr = malloc(%zu); //%p\n", event->size, event->addr);
		case MALT_TRACE_EVENT_C_CALLOC:
			return snprintf(buffer, buffer_size, "ptr = calloc(%zu, %zu); //%p\n", event->size, event->extra.calloc.nmemb, event->addr);
		case MALT_TRACE_EVENT_C_REALLOC:
			return snprintf(buffer, buffer_size, "ptr = realloc(%p, %zu); //%p (old_size=%zu)\n", event->extra.realloc.oldAddr, event->size, event->addr, event->extra.realloc.oldSize);
		case MALT_TRACE_EVENT_C_FREE:
			return snprintf(buffer, buffer_size, "free(%p); //(old_size=%zu, lifetime=%zu)\n", event->addr, event->size, event->extra.free.lifetime);
		case MALT_TRACE_EVENT_C_POSIX_MEMALIGN:
			return snprintf(buffer, buffer_size, "status = posix_memalign(&ptr, %zu, %zu); //(ptr=%p)\n", event->extra.memalign.align, event->size, event->addr);
		case MALT_TRACE_EVENT_C_ALIGNED_ALLOC:
			return snprintf(buffer, buffer_size, "ptr = aligned_alloc(%zu, %zu); //(ptr=%p)\n", event->extra.memalign.align, event->size, event->addr);
		case MALT_TRACE_EVENT_C_MEMALIGN:
			return snprintf(buffer, buffer_size, "ptr = memalign(%zu, %zu); //(ptr=%p)\n", event->extra.memalign.align, event->size, event->addr);
		case MALT_TRACE_EVENT_C_VALLOC:
			return snprintf(buffer, buffer_size, "ptr = valloc(%zu); //(ptr=%p)\n", event->size, event->addr);
		case MALT_TRACE_EVENT_C_PVALLOC:
			return snprintf(buffer, buffer_size, "ptr = pvalloc(%zu); //(ptr=%p)\n", event->size, event->addr);
		//lifetime
		case MALT_TRACE_EVENT_C_REALLOC_LFTIME:
			return snprintf(buffer, buffer_size, "//realloc-free(%p); //(old_size=%zu, lifetime=%zu)\n", event->addr, event->size, event->extra.reallocLifetime.lifetime);
		//c mmap
		case MALT_TRACE_EVENT_C_MMAP:
			return snprintf(buffer, buffer_size, "ptr = mmap(%p, %zu); //(ptr=%p)\n", event->addr, event->size, event->addr);
		case MALT_TRACE_EVENT_C_MUNMAP:
			return snprintf(buffer, buffer_size, "munmap(%p, %zu);\n", event->addr, event->size);
		case MALT_TRACE_EVENT_C_MREMAP:
			return snprintf(buffer, buffer_size, "ptr = mremap(%p, %zu, %zu, %p)", event->addr, event->size, event->extra.mremap.newSize, event->extra.mremap.newAddr);
		//python obj
		case MALT_TRACE_EVENT_PY_OBJ_MALLOC:
			return snprintf(buffer, buffer_size, "ptr = PyObjMalloc(%zu); //%p\n", event->size, event->addr);
		case MALT_TRACE_EVENT_PY_OBJ_FREE:
			return snprintf(buffer, buffer_size, "PyObjFree(%p); //(old_size=%zu, lifetime=%zu)\n", event->addr, event->size, event->extra.free.lifetime);
		case MALT_TRACE_EVENT_PY_OBJ_REALLOC:
			return snprintf(buffer, buffer_size, "ptr = PyObjRealloc(%p, %zu); //%p (old_size=%zu)\n", event->extra.realloc.oldAddr, event->size, event->addr, event->extra.realloc.oldSize);
		case MALT_TRACE_EVENT_PY_OBJ_CALLOC:
			return snprintf(buffer, buffer_size, "ptr = PyObjCalloc(%zu, %zu); //%p\n", event->size, event->extra.calloc.nmemb, event->addr);
		//python mem
		case MALT_TRACE_EVENT_PY_MEM_MALLOC:
			return snprintf(buffer, buffer_size, "ptr = PyMemMalloc(%zu); //%p\n", event->size, event->addr);
		case MALT_TRACE_EVENT_PY_MEM_FREE:
			return snprintf(buffer, buffer_size, "PyMemFree(%p); //(old_size=%zu, lifetime=%zu)\n", event->addr, event->size, event->extra.free.lifetime);
		case MALT_TRACE_EVENT_PY_MEM_REALLOC:
			return snprintf(buffer, buffer_size, "ptr = PyMemRealloc(%p, %zu); //%p (old_size=%zu)\n", event->extra.realloc.oldAddr, event->size, event->addr, event->extra.realloc.oldSize);
		case MALT_TRACE_EVENT_PY_MEM_CALLOC:
			return snprintf(buffer, buffer_size, "ptr = PyMemCalloc(%zu, %zu); //%p\n", event->size, event->extra.calloc.nmemb, event->addr);
		//pyton raw
		case MALT_TRACE_EVENT_PY_RAW_MALLOC:
			return snprintf(buffer, buffer_size, "ptr = PyRawMalloc(%zu); //%p\n", event->size, event->addr);
		case MALT_TRACE_EVENT_PY_RAW_FREE:
			return snprintf(buffer, buffer_size, "PyRawFree(%p); //(old_size=%zu, lifetime=%zu)\n", event->addr, event->size, event->extra.free.lifetime);
		case MALT_TRACE_EVENT_PY_RAW_REALLOC:
			return snprintf(buffer, buffer_size, "ptr = PyRawRealloc(%p, %zu); //%p (old_size=%zu)\n", event->extra.realloc.oldAddr, event->size, event->addr, event->extra.realloc.oldSize);
		case MALT_TRACE_EVENT_PY_RAW_CALLOC:
			return snprintf(buffer, buffer_size, "ptr = PyRawCalloc(%zu, %zu); //%p\n", event->size, event->extra.calloc.nmemb, event->addr);
		default:
			return snprintf(buffer, buffer_size, "/*TODO*/");
	}
}

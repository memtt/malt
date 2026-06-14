/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 04/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/trace/ProcessMemModel.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2026
***********************************************************/

/**********************************************************/
#include <cassert>
#include "ProcessMemModel.hpp"

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
ProcessMemModel::ProcessMemModel(void)
{
	this->inheritance[PROCESS_MEM_DOMAIN_MALLOC].push_back(PROCESS_MEM_DOMAIN_C);
	this->inheritance[PROCESS_MEM_DOMAIN_MALLOC].push_back(PROCESS_MEM_DOMAIN_ALL);
	this->inheritance[PROCESS_MEM_DOMAIN_MMAP].push_back(PROCESS_MEM_DOMAIN_C);
	this->inheritance[PROCESS_MEM_DOMAIN_MMAP].push_back(PROCESS_MEM_DOMAIN_ALL);
	this->inheritance[PROCESS_MEM_DOMAIN_PYTHON_OBJ].push_back(PROCESS_MEM_DOMAIN_PYTHON);
	this->inheritance[PROCESS_MEM_DOMAIN_PYTHON_OBJ].push_back(PROCESS_MEM_DOMAIN_ALL);
	this->inheritance[PROCESS_MEM_DOMAIN_PYTHON_MEM].push_back(PROCESS_MEM_DOMAIN_PYTHON);
	this->inheritance[PROCESS_MEM_DOMAIN_PYTHON_MEM].push_back(PROCESS_MEM_DOMAIN_ALL);
	this->inheritance[PROCESS_MEM_DOMAIN_PYTHON_RAW].push_back(PROCESS_MEM_DOMAIN_PYTHON);
	this->inheritance[PROCESS_MEM_DOMAIN_PYTHON_RAW].push_back(PROCESS_MEM_DOMAIN_ALL);
}

/**********************************************************/
ProcessMemModel::~ProcessMemModel(void)
{
}

/**********************************************************/
void ProcessMemModel::playNextEvent(MALT::AllocTracerEvent & event)
{
	//cases
	switch (event.type)
	{
		//NOP
		case MALT_TRACE_EVENT_NOP:
			break;
		//C malloc
		case MALT_TRACE_EVENT_C_MALLOC:
			this->incr(PROCESS_MEM_DOMAIN_MALLOC, 1, event.size);
			break;
		case MALT_TRACE_EVENT_C_CALLOC:
			this->incr(PROCESS_MEM_DOMAIN_MALLOC, 1, event.size);
			break;
		case MALT_TRACE_EVENT_C_REALLOC: {
			if (event.size >= event.extra.realloc.oldSize)
				this->incr(PROCESS_MEM_DOMAIN_MALLOC, 0, event.size - event.extra.realloc.oldSize);
			else
				this->decr(PROCESS_MEM_DOMAIN_MALLOC, 0, event.extra.realloc.oldSize - event.size);
			break;
		}
		case MALT_TRACE_EVENT_C_FREE:
			this->decr(PROCESS_MEM_DOMAIN_MALLOC, 1, event.size);
			break;
		case MALT_TRACE_EVENT_C_POSIX_MEMALIGN:
			this->incr(PROCESS_MEM_DOMAIN_MALLOC, 1, event.size);
			break;
		case MALT_TRACE_EVENT_C_ALIGNED_ALLOC:
			this->incr(PROCESS_MEM_DOMAIN_MALLOC, 1, event.size);
			break;
		case MALT_TRACE_EVENT_C_MEMALIGN:
			this->incr(PROCESS_MEM_DOMAIN_MALLOC, 1, event.size);
			break;
		case MALT_TRACE_EVENT_C_VALLOC:
			this->incr(PROCESS_MEM_DOMAIN_MALLOC, 1, event.size);
			break;
		case MALT_TRACE_EVENT_C_PVALLOC:
			this->incr(PROCESS_MEM_DOMAIN_MALLOC, 1, event.size);
			break;
		//lifetime
		case MALT_TRACE_EVENT_C_REALLOC_LFTIME:
			this->decr(PROCESS_MEM_DOMAIN_MALLOC, 1, 0);
			break;
		//c mmap
		case MALT_TRACE_EVENT_C_MMAP:
			this->incr(PROCESS_MEM_DOMAIN_MMAP, 0, event.size);
			break;
		case MALT_TRACE_EVENT_C_MUNMAP:
			this->decr(PROCESS_MEM_DOMAIN_MMAP, 0, event.size);
			break;
		case MALT_TRACE_EVENT_C_MREMAP: {
			if (event.extra.mremap.newSize >= event.size)
				this->incr(PROCESS_MEM_DOMAIN_MMAP, 0, event.extra.mremap.newSize - event.size);
			else
				this->decr(PROCESS_MEM_DOMAIN_MMAP, 0, event.size - event.extra.mremap.newSize);
			break;
		}
		//python obj
		case MALT_TRACE_EVENT_PY_OBJ_MALLOC:
			this->incr(PROCESS_MEM_DOMAIN_PYTHON_OBJ, 1, event.size);
			break;
		case MALT_TRACE_EVENT_PY_OBJ_FREE:
			this->decr(PROCESS_MEM_DOMAIN_PYTHON_OBJ, 1, event.size);
			break;
		case MALT_TRACE_EVENT_PY_OBJ_REALLOC: {
			if (event.size >= event.extra.realloc.oldSize)
				this->incr(PROCESS_MEM_DOMAIN_PYTHON_OBJ, 0, event.size - event.extra.realloc.oldSize);
			else
				this->decr(PROCESS_MEM_DOMAIN_PYTHON_OBJ, 0, event.extra.realloc.oldSize - event.size);
			break;
		}
		case MALT_TRACE_EVENT_PY_OBJ_CALLOC:
			this->incr(PROCESS_MEM_DOMAIN_PYTHON_OBJ, 1, event.size);
			break;
		//python mem
		case MALT_TRACE_EVENT_PY_MEM_MALLOC:
			this->incr(PROCESS_MEM_DOMAIN_PYTHON_MEM, 1, event.size);
			break;
		case MALT_TRACE_EVENT_PY_MEM_FREE:
			this->decr(PROCESS_MEM_DOMAIN_PYTHON_MEM, 1, event.size);
			break;
		case MALT_TRACE_EVENT_PY_MEM_REALLOC: {
			if (event.size >= event.extra.realloc.oldSize)
				this->incr(PROCESS_MEM_DOMAIN_PYTHON_MEM, 0, event.size - event.extra.realloc.oldSize);
			else
				this->decr(PROCESS_MEM_DOMAIN_PYTHON_MEM, 0, event.extra.realloc.oldSize - event.size);
			break;
		}
		case MALT_TRACE_EVENT_PY_MEM_CALLOC:
			this->incr(PROCESS_MEM_DOMAIN_PYTHON_MEM, 1, event.size);
			break;
		//pyton raw
		case MALT_TRACE_EVENT_PY_RAW_MALLOC:
			this->incr(PROCESS_MEM_DOMAIN_PYTHON_RAW, 1, event.size);
			break;
		case MALT_TRACE_EVENT_PY_RAW_FREE:
			this->decr(PROCESS_MEM_DOMAIN_PYTHON_RAW, 1, event.size);
			break;
		case MALT_TRACE_EVENT_PY_RAW_REALLOC: {
			if (event.size >= event.extra.realloc.oldSize)
				this->incr(PROCESS_MEM_DOMAIN_PYTHON_RAW, 0, event.size - event.extra.realloc.oldSize);
			else
				this->decr(PROCESS_MEM_DOMAIN_PYTHON_RAW, 0, event.extra.realloc.oldSize - event.size);
			break;
		}
		case MALT_TRACE_EVENT_PY_RAW_CALLOC:
			this->incr(PROCESS_MEM_DOMAIN_PYTHON_RAW, 1, event.size);
			break;
		default:
			assert(false);
			break;
	}
}

/**********************************************************/
ProcessMemDomainCounter ProcessMemModel::getCounters(ProcessMemDomain domain)
{
	assert(domain >= 0 && domain <= PROCESS_MEM_DOMAIN_ALL);
	return this->counters[domain];
}

/**********************************************************/
void ProcessMemModel::incr(ProcessMemDomain domain, size_t count, size_t mem)
{
	//check
	assert(domain >= 0 && domain <= PROCESS_MEM_DOMAIN_ALL);

	//default
	this->counters[domain].aliveChunks += count;
	this->counters[domain].usedVirtualMemory += mem;

	//derivates
	for (const auto parentDomain : this->inheritance[domain]) {
		this->counters[parentDomain].aliveChunks += count;
		this->counters[parentDomain].usedVirtualMemory += mem;
	}
}

/**********************************************************/
void ProcessMemModel::decr(ProcessMemDomain domain, size_t count, size_t mem)
{
	//check
	assert(domain >= 0 && domain <= PROCESS_MEM_DOMAIN_ALL);

	//default
	assert(this->counters[domain].aliveChunks >= count);
	assert(this->counters[domain].usedVirtualMemory >= mem);
	this->counters[domain].aliveChunks -= count;
	this->counters[domain].usedVirtualMemory -= mem;

	//derivates
	for (const auto parentDomain : this->inheritance[domain]) {
		assert(this->counters[parentDomain].aliveChunks >= count);
		assert(this->counters[parentDomain].usedVirtualMemory >= mem);
		this->counters[parentDomain].aliveChunks -= count;
		this->counters[parentDomain].usedVirtualMemory -= mem;
	}
}

}

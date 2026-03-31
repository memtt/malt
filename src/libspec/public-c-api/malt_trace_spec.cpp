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
#include "malt_trace_spec.hpp"

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

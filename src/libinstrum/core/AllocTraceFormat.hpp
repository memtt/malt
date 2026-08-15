/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 04/2026
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
#include "public-c-api/malt-trace-spec.h"

/**********************************************************/
namespace MALT
{

/**********************************************************/
typedef malt_trace_event_type_t AllocTraceEventType;

/**********************************************************/
/**
 * Define an event to dump in binary format in the trace file.
 * 
 * @brief Structure dumped to allocation trace files.
**/
class AllocTracerEvent : public malt_trace_event_t
{
	public:
		inline AllocTracerEvent(const malt_trace_event_t & orig);
		inline AllocTracerEvent(void);
		inline size_t toCString(char * buffer, size_t bufferSize) const;
		inline bool fromCString(const char * buffer);
		inline bool fromFilePointer(FILE * fp);
};

/**********************************************************/
inline AllocTracerEvent::AllocTracerEvent(const malt_trace_event_t & orig)
{
	*this = orig;
}

/**********************************************************/
inline AllocTracerEvent::AllocTracerEvent(void)
{
	malt_trace_event_init(this);
}

/**********************************************************/
inline size_t AllocTracerEvent::toCString(char * buffer, size_t bufferSize) const
{
	return malt_trace_event_to_string(this, buffer, bufferSize);
}

/**********************************************************/
inline bool AllocTracerEvent::fromCString(const char * buffer)
{
	return malt_trace_event_from_string(this, buffer);
}

/**********************************************************/
inline bool AllocTracerEvent::fromFilePointer(FILE * fp)
{
	return malt_trace_event_from_fp(this, fp);
}

}

#endif //MALT_ALLOC_TRACE_FORMAT_HPP

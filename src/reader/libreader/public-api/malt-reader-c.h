/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/public-api/malt-reader.h
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
*    AUTHOR   : Sébastien Valat - 2026
***********************************************************/

#ifndef MALT_READER_C_H
#define MALT_READER_C_H

/**********************************************************/
#ifdef __cplusplus
	#include <string>
	#include <cstdlib>
#else
	#include <stdlib.h>
	#include <stdbool.h>
#endif
#include "malt-profile-spec.h"
#include "malt-trace-spec.h"
#include "malt-reader.h"

/**********************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************/
struct malt_stack_infos_s
{
	size_t uid;
	malt_profile_lang_address_t lang_addr;
	const char * file;
	const char * binary;
	const char * function;
	const char * functionShort;
	ssize_t line;
	size_t offset;
};
typedef struct malt_stack_infos_s malt_stack_infos_t;

/**********************************************************/
//traces
size_t malt_reader_c_get_trace_count(struct malt_reader_t * reader);
const malt_trace_event_t * malt_reader_c_get_trace_event(struct malt_reader_t * reader, size_t id);

/**********************************************************/
//stack
size_t malt_reader_c_get_stack_size(struct malt_reader_t * reader, const malt_stack_t * stack);
size_t malt_reader_c_load_stack_infos(struct malt_reader_t * reader, const malt_stack_t * stack, malt_stack_infos_t * stack_infos, size_t stack_info_length);

/**********************************************************/
//sources
bool malt_reader_c_is_source_file(struct malt_reader_t * reader, const char * path);

/**********************************************************/
#ifdef __cplusplus
}
#endif

#endif //MALT_READER_C_H

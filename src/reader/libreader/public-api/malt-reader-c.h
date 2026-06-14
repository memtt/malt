/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 04/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/public-api/malt-reader-c.h
*-----------------------------------------------------------
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
struct malt_process_mem_model_s;
typedef struct malt_process_mem_model_s malt_process_mem_model_t;

/**********************************************************/
struct malt_process_mem_counter_s
{
	size_t alive_chunks;
	size_t virtual_memory;
};
typedef struct malt_process_mem_counter_s malt_process_mem_counter_t;

/**********************************************************/
enum malt_process_mem_domain_e
{
	MALT_PROCESS_MEM_DOMAIN_MALLOC,
	MALT_PROCESS_MEM_DOMAIN_MMAP,
	MALT_PROCESS_MEM_DOMAIN_C,
	MALT_PROCESS_MEM_DOMAIN_PYTHON_OBJ,
	MALT_PROCESS_MEM_DOMAIN_PYTHON_MEM,
	MALT_PROCESS_MEM_DOMAIN_PYTHON_RAW,
	MALT_PROCESS_MEM_DOMAIN_PYTHON,
	MALT_PROCESS_MEM_DOMAIN_ALL
};
typedef enum malt_process_mem_domain_e malt_process_mem_domain_t;

/**********************************************************/
//traces
size_t malt_reader_c_get_trace_count(struct malt_reader_t * reader);
const malt_trace_event_t * malt_reader_c_get_trace_event(struct malt_reader_t * reader, size_t id);

/**********************************************************/
//memory model
malt_process_mem_model_t * malt_reader_c_process_mem_model_create(void);
void malt_reader_c_process_mem_model_play_event(malt_process_mem_model_t * model, const malt_trace_event_t * event);
malt_process_mem_counter_t malt_reader_c_process_mem_model_get(malt_process_mem_model_t * model, malt_process_mem_domain_t domain);
void malt_reader_c_process_mem_model_delete(malt_process_mem_model_t * model);


/**********************************************************/
//stack
size_t malt_reader_c_get_stack_size(struct malt_reader_t * reader, const malt_stack_t * stack);
bool malt_reader_c_load_stack_infos(struct malt_reader_t * reader, const malt_stack_t * stack, malt_stack_infos_t * stack_infos, size_t id);

/**********************************************************/
//sources
bool malt_reader_c_is_source_file(struct malt_reader_t * reader, const char * path);

/**********************************************************/
#ifdef __cplusplus
}
#endif

#endif //MALT_READER_C_H

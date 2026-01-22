/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 10/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/public-api/malt-reader.h
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_READER_H
#define MALT_READER_H

/**********************************************************/
#ifdef __cplusplus
	#include <string>
	#include <cstdlib>
#else
	#include <stdlib.h>
	#include <stdbool.h>
#endif

/**********************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************/
enum malt_reader_flags_t
{
	MALT_READER_NONE=0,
	MALT_READER_PROGRESS_BAR=1,
	MALT_READER_VERBOSE=2,
};

/**********************************************************/
struct malt_reader_t;

/**********************************************************/
// lifecycle
struct malt_reader_t * malt_reader_init(const char * fname, size_t flags);
void malt_reader_fini(struct malt_reader_t * reader);
const char * malt_reader_version(void);

/**********************************************************/
bool malt_reader_is_source_file(struct malt_reader_t * reader, const char * path);

/**********************************************************/
// requests
char * malt_reader_json_get_proc_map(struct malt_reader_t * reader);
char * malt_reader_json_get_debug_stack_list(struct malt_reader_t * reader);
char * malt_reader_json_get_global_variables(struct malt_reader_t * reader);
char * malt_reader_json_get_summary(struct malt_reader_t * reader);
char * malt_reader_json_get_summary_v2(struct malt_reader_t * reader);
char * malt_reader_json_get_file_lines_flat_profile(struct malt_reader_t * reader, const char * file, bool total);
char * malt_reader_json_get_flat_function_profile(struct malt_reader_t * reader, bool own, bool total);
char * malt_reader_json_get_binary_addresses_profile(struct malt_reader_t* reader, const char * binary, void ** addresses, size_t addr_count, bool total);
char * malt_reader_json_get_binary_addresses_profile_all(struct malt_reader_t* reader, const char * binary, bool total);
char * malt_reader_json_get_proc_map_distr(struct malt_reader_t * reader);
char * malt_reader_json_get_scatter(struct malt_reader_t * reader);
char * malt_reader_json_get_size_map(struct malt_reader_t * reader);
char * malt_reader_json_get_realloc_map(struct malt_reader_t * reader);
char * malt_reader_json_get_timed_values(struct malt_reader_t * reader);
char * malt_reader_json_get_filtered_stacks_on_file_line(struct malt_reader_t * reader, const char * file, size_t line);
char * malt_reader_json_get_filterd_stacks_on_symbol(struct malt_reader_t * reader, const char * func);
char * malt_reader_json_get_call_stack_next_level(struct malt_reader_t * reader, size_t parentStackId, size_t parentDepth, const char * function, const char * file, int line);
char * malt_reader_json_get_stacks_mem(struct malt_reader_t * reader);
char * malt_reader_json_get_stack_info_on_function(struct malt_reader_t * reader, size_t thread_id);
char * malt_reader_json_get_call_tree(struct malt_reader_t * reader, ssize_t nodeId, ssize_t depth, ssize_t height, double min_cost, const char * func, const char * metric, bool is_ratio);
char * malt_reader_json_get_static_full(struct malt_reader_t * reader);

/**********************************************************/
//via JSON request
char * malt_reader_json_request(struct malt_reader_t* reader, const char * request, bool * continue_run);

/**********************************************************/
#ifdef __cplusplus
}
#endif

/**********************************************************/
#ifdef __cplusplus
std::string malt_reader_json_request_cpp(struct malt_reader_t * reader, const std::string & request, bool * continue_run = nullptr);
#endif

#endif //MALT_READER_H

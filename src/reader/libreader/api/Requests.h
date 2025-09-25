/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/api/Requests.h
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_READER_REQUESTS_HPP
#define MALT_READER_REQUESTS_HPP

/**********************************************************/
#include <cstdlib>
#ifdef __cplusplus
	#include <string>
#endif

/**********************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************/
struct malt_reader_t;

/**********************************************************/
// lifecycle
malt_reader_t * malt_reader_init(const char * fname, bool progress_bar, bool auto_delete);
void malt_reader_fini(malt_reader_t * reader);
void malt_reader_free_response(const char * response);
void malt_reader_free_last_response(malt_reader_t * reader);
// requests
const char * malt_reader_get_proc_map(malt_reader_t * reader);
const char * malt_reader_get_debug_stack_list(malt_reader_t * reader);
const char * malt_reader_get_global_variables(malt_reader_t * reader);
const char * malt_reader_get_summary(malt_reader_t * reader);
const char * malt_reader_get_summary_v2(malt_reader_t * reader);
const char * malt_reader_get_file_lines_flat_profile(malt_reader_t * reader, const char * file, bool total);
const char * malt_reader_get_flat_function_profile(malt_reader_t * reader, bool own, bool total);
const char * malt_reader_get_proc_map_distr(malt_reader_t * reader);
const char * malt_reader_get_scatter(malt_reader_t * reader);
const char * malt_reader_get_size_map(malt_reader_t * reader);
const char * malt_reader_get_realloc_map(malt_reader_t * reader);
const char * malt_reader_get_timed_values(malt_reader_t * reader);
const char * malt_reader_get_filtered_stacks_on_file_line(malt_reader_t * reader, const char * file, size_t line);
const char * malt_reader_get_filterd_stacks_on_symbol(malt_reader_t * reader, const char * func);
const char * malt_reader_get_call_stack_next_level(malt_reader_t * reader, size_t parentStackId, size_t parentDepth, const char * function, const char * file, int line);
const char * malt_reader_get_stacks_mem(malt_reader_t * reader);
const char * malt_reader_get_stack_info_on_function(malt_reader_t * reader, size_t thread_id);
const char * malt_reader_get_call_tree(malt_reader_t * reader, ssize_t nodeId, ssize_t depth, ssize_t height, double min_cost, const char * func, const char * metric, bool is_ratio);
bool malt_reader_is_source_file(malt_reader_t * reader, const char * path);
//via JSON request
const char * malt_reader_json_request(malt_reader_t * reader, const char * request);

/**********************************************************/
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
std::string malt_reader_json_request_cpp(malt_reader_t * reader, const std::string & request, bool * continue_run = nullptr);
#endif

#endif //MALT_READER_REQUESTS_HPP

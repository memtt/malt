/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/Backtrace.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_READER_REQUESTS_HPP
#define MALT_READER_REQUESTS_HPP

/**********************************************************/
#include <cstdlib>

/**********************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************/
struct malt_reader_t;

/**********************************************************/
// lifecycle
malt_reader_t * malt_reader_init(const char * fname);
void malt_reader_destroy(malt_reader_t * reader);
void malt_reader_free_response(const char * response);
// requests
const char * malt_reader_get_summary(malt_reader_t * reader);
const char * malt_reader_get_summary_v2(malt_reader_t * reader);
const char * malt_reader_get_flat_profile(malt_reader_t * reader);

/**********************************************************/
#ifdef __cplusplus
}
#endif

#endif //MALT_READER_REQUESTS_HPP

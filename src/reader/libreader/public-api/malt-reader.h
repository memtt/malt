/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/public-api/malt-reader.h
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
*    AUTHOR   : Sébastien Valat - 2026
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
#ifdef __cplusplus
}
#endif

#endif //MALT_READER_H

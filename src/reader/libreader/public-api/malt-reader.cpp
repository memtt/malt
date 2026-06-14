/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 04/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/public-api/malt-reader.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
*    AUTHOR   : Sébastien Valat - 2026
***********************************************************/

/**********************************************************/
#include "config.h"
#include <cstdio>
#include <functional>
#include <nlohmann/json.hpp>
#include <iostream>
#include "../api/WebProfile.hpp"
#include "malt-reader.h"
#include "malt-reader-internal.h"
#include "libinstrum/portability/Visibility.hpp"

/**********************************************************/
using namespace MALTReader;

/**********************************************************/
DLL_PUBLIC malt_reader_t * malt_reader_init(const char * fname, size_t flags)
{
	//config
	const bool progress_bar = flags & MALT_READER_PROGRESS_BAR;
	const bool verbose =  flags & MALT_READER_VERBOSE;

	try {
		if (verbose)
			fprintf(stderr, "[MALT-WEBVIEW2] Loading : %s\n", fname);
		WebProfile * profile = new WebProfile(fname, progress_bar);
		malt_reader_t * reader = new malt_reader_t();
		reader->profile = profile;
		reader->verbose = verbose;
		if (verbose)
			fprintf(stderr, "[MALT-WEBVIEW2] Loading OK => %p\n", (void*)reader);
		return reader;
	} catch(std::exception & e) {
		fprintf(stderr, "[MALT-WEBVIEW2] Fail to load profile : %s\n", fname);
		return nullptr;
	}
}

/**********************************************************/
DLL_PUBLIC void malt_reader_fini(struct malt_reader_t* reader)
{
	// trivial
	if (reader == nullptr)
		return;

	// ok
	delete reader->profile;
	delete reader;
}

/**********************************************************/
DLL_PUBLIC void malt_reader_set_verbose(struct malt_reader_t * reader, bool verbose)
{
	if (reader == nullptr)
		return;
	reader->verbose = verbose;
}

/**********************************************************/
DLL_PUBLIC const char * malt_reader_version(void)
{
	return MALT_VERSION;
}

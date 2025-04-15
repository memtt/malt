/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/Backtrace.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <cstdio>
#include <nlohmann/json.hpp>
#include "WebProfile.hpp"
#include "Requests.h"

/**********************************************************/
using namespace MALTReader;

/**********************************************************/
struct malt_reader_t
{
	WebProfile * profile{nullptr};
};

/**********************************************************/
malt_reader_t * malt_reader_init(const char * fname)
{
	try {
		fprintf(stderr, "[MALT-WEBVIEW2] Loading : %s\n", fname);
		WebProfile * profile = new WebProfile(fname, true);
		malt_reader_t * reader = new malt_reader_t();
		reader->profile = profile;
		fprintf(stderr, "[MALT-WEBVIEW2] Loading OK => %p\n", reader);
		return reader;
	} catch(std::exception & e) {
		fprintf(stderr, "[MALT-WEBVIEW2] Fail to load profile : %s\n", fname);
		return nullptr;
	}
}

/**********************************************************/
void malt_reader_destroy(malt_reader_t * reader)
{
	// trivial
	if (reader == nullptr)
		return;

	// ok
	delete reader->profile;
	delete reader;
}

/**********************************************************/
const char * malt_reader_get_summary(malt_reader_t * reader)
{
	fprintf(stderr, "[MALT-WEBVIEW-2] Request GET_SUMMARY(%p)\n", reader);
	nlohmann::json json = reader->profile->getSummary();
	std::string str = json.dump();
	const char * res = strdup(str.c_str());
	fprintf(stderr, "[MALT-WEBVIEW-2] Response : %s\n", res);
	return res;
}

/**********************************************************/
const char * malt_reader_get_summary_v2(malt_reader_t * reader)
{
	fprintf(stderr, "[MALT-WEBVIEW-2] Request GET_SUMMARY_V2(%p)\n", reader);
	nlohmann::json json = reader->profile->getSummaryV2();
	std::string str = json.dump();
	const char * res = strdup(str.c_str());
	fprintf(stderr, "[MALT-WEBVIEW-2] Response : %p = %s\n", res, res);
	return res;
}

/**********************************************************/
void malt_reader_free_response(const char * response)
{
	fprintf(stderr, "[MALT-WEBVIEW-2] free(%p)\n", response);
	free((void*)response);
}

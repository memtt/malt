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
	const char * last_response{nullptr};
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

	//free
	malt_reader_free_last_response(reader);

	// ok
	delete reader->profile;
	delete reader;
}

/**********************************************************/
void malt_reader_free_last_response(malt_reader_t * reader)
{
	free((void*)(reader->last_response));
	reader->last_response = nullptr;
}

/**********************************************************/
const char * malt_reader_get_summary(malt_reader_t * reader)
{
	try {
		fprintf(stderr, "[MALT-WEBVIEW-2] Request GET_SUMMARY(%p)\n", reader);
		malt_reader_free_last_response(reader);
		nlohmann::json json = reader->profile->getSummary();
		std::string str = json.dump();
		const char * res = strdup(str.c_str());
		reader->last_response = res;
		//fprintf(stderr, "[MALT-WEBVIEW-2] Response : %s\n", res);
		return res;
	} catch (std::exception & e) {
		return nullptr;
	}
}

/**********************************************************/
const char * malt_reader_get_summary_v2(malt_reader_t * reader)
{
	try {
		fprintf(stderr, "[MALT-WEBVIEW-2] Request GET_SUMMARY_V2(%p)\n", reader);
		malt_reader_free_last_response(reader);
		nlohmann::json json = reader->profile->getSummaryV2();
		std::string str = json.dump();
		const char * res = strdup(str.c_str());
		reader->last_response = res;
		//fprintf(stderr, "[MALT-WEBVIEW-2] Response : %p = %s\n", res, res);
		return res;
	} catch (std::exception & e) {
		return nullptr;
	}
}


/**********************************************************/
const char * malt_reader_get_flat_profile(malt_reader_t * reader)
{
	try {
		fprintf(stderr, "[MALT-WEBVIEW-2] Request get flat profile(%p)\n", reader);
		malt_reader_free_last_response(reader);
		nlohmann::json json = reader->profile->getFlatFunctionProfile(true, true);
		std::string str = json.dump();
		const char * res = strdup(str.c_str());
		//fprintf(stderr, "[MALT-WEBVIEW-2] Response : %p = %s\n", res, res);
		reader->last_response = res;
		return res;
	} catch (std::exception & e) {
		return nullptr;
	}
}

/**********************************************************/
void malt_reader_free_response(const char * response)
{
	fprintf(stderr, "[MALT-WEBVIEW-2] free(%p)\n", response);
	free((void*)response);
}

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
#include <functional>
#include <nlohmann/json.hpp>
#include <iostream>
#include "WebProfile.hpp"
#include "Requests.h"

/**********************************************************/
using namespace MALTReader;

/**********************************************************/
struct malt_reader_t
{
	WebProfile * profile{nullptr};
	const char * last_response{nullptr};
	bool auto_delete{false};
};

/**********************************************************/
malt_reader_t * malt_reader_init(const char * fname, bool progress_bar, bool auto_delete)
{
	try {
		fprintf(stderr, "[MALT-WEBVIEW2] Loading : %s\n", fname);
		WebProfile * profile = new WebProfile(fname, progress_bar);
		malt_reader_t * reader = new malt_reader_t();
		reader->profile = profile;
		reader->auto_delete = auto_delete;
		fprintf(stderr, "[MALT-WEBVIEW2] Loading OK => %p\n", reader);
		return reader;
	} catch(std::exception & e) {
		fprintf(stderr, "[MALT-WEBVIEW2] Fail to load profile : %s\n", fname);
		return nullptr;
	}
}

/**********************************************************/
void malt_reader_fini(malt_reader_t * reader)
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
	//check
	assert(reader != nullptr);

	//if needs
	if (reader->auto_delete) {
		free((void*)(reader->last_response));
		reader->last_response = nullptr;
	}
}

/**********************************************************/
const char * malt_reader_hanle_request(malt_reader_t * reader, const char * name, std::function<nlohmann::json()> handler)
{
	try {
		fprintf(stderr, "[MALT-WEBVIEW-2] Request %s(%p)\n", name, reader);
		malt_reader_free_last_response(reader);
		nlohmann::json json = handler();
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
const char * malt_reader_get_proc_map(malt_reader_t * reader)
{
	return malt_reader_hanle_request(reader, "GET_PROC_MAP", [reader]() {
		return reader->profile->getProcMap();
	});
}

/**********************************************************/
const char * malt_reader_get_debug_stack_list(malt_reader_t * reader)
{
	return malt_reader_hanle_request(reader, "GET_DEBUG_STACK_LIST", [reader]() {
		return reader->profile->getDebugStackList();
	});
}

/**********************************************************/
const char * malt_reader_get_global_variables(malt_reader_t * reader)
{
	return malt_reader_hanle_request(reader, "GEt_GLOBAL_VARIABLES", [reader]() {
		return reader->profile->getGlobalVariables();
	});
}

/**********************************************************/
const char * malt_reader_get_summary(malt_reader_t * reader)
{
	return malt_reader_hanle_request(reader, "GET_SUMMARY", [reader]() {
		return reader->profile->getSummary();
	});
}

/**********************************************************/
const char * malt_reader_get_summary_v2(malt_reader_t * reader)
{
	return malt_reader_hanle_request(reader, "GET_SUMMARY_V2", [reader]() {
		return reader->profile->getSummaryV2();
	});
}

/**********************************************************/
const char * malt_reader_get_file_lines_flat_profile(malt_reader_t * reader, const char * file, bool total)
{
	return malt_reader_hanle_request(reader, "GET_FILE_LINES_FLAT_PROFILE", [reader, file, total]() {
		return reader->profile->getFileLinesFlatProfile(file, total);
	});
}

/**********************************************************/
const char * malt_reader_get_flat_function_profile(malt_reader_t * reader, bool own, bool total)
{
	return malt_reader_hanle_request(reader, "GET_FILE_LINES_FLAT_PROFILE", [reader, own, total]() {
		return reader->profile->getFlatFunctionProfile(own, total);
	});
}

/**********************************************************/
const char * malt_reader_get_proc_map_distr(malt_reader_t * reader)
{
	return malt_reader_hanle_request(reader, "GET_PROC_MAP_DISTR", [reader]() {
		return reader->profile->getProcMapDistr();
	});
}

/**********************************************************/
const char * malt_reader_get_scatter(malt_reader_t * reader)
{
	return malt_reader_hanle_request(reader, "GET_SCATTER", [reader]() {
		return reader->profile->getScatter();
	});
}

/**********************************************************/
const char * malt_reader_get_size_map(malt_reader_t * reader)
{
	return malt_reader_hanle_request(reader, "GET_SIZE_MAP", [reader]() {
		return reader->profile->getSizeMap();
	});
}

/**********************************************************/
const char * malt_reader_get_realloc_map(malt_reader_t * reader)
{
	return malt_reader_hanle_request(reader, "GET_REALLOC_MAP", [reader]() {
		return reader->profile->getReallocMap();
	});
}

/**********************************************************/
const char * malt_reader_get_timed_values(malt_reader_t * reader)
{
	return malt_reader_hanle_request(reader, "GET_TIMED_VALUES", [reader]() {
		return reader->profile->getTimedValues();
	});
}

/**********************************************************/
const char * malt_reader_get_filtered_stacks_on_file_line(malt_reader_t * reader, const char * file, size_t line)
{
	return malt_reader_hanle_request(reader, "GET_FILTERED_STACKS_ON_FILE_LINE", [reader, file, line]() {
		return reader->profile->getFileLinesFlatProfile(file, line);
	});
}

/**********************************************************/
const char * malt_reader_get_filterd_stacks_on_symbol(malt_reader_t  *reader, const char * func)
{
	return malt_reader_hanle_request(reader, "GET_FILTERED_STACKS_ON_SYMBOL", [reader, func]() {
		return reader->profile->getFilterdStacksOnSymbol(func);
	});
}

/**********************************************************/
const char * malt_reader_get_call_stack_next_level(malt_reader_t * reader, size_t parentStackId, size_t parentDepth, const char * function, const char * file, int line)
{
	return malt_reader_hanle_request(reader, "GET_CALL_STACK_NEXT_LEVEL", [reader, parentStackId, parentDepth, file, function, line]() {
		//build filter
		LocationFilter filter;
		if (function != nullptr)
			filter.function = function;
		if (file != nullptr)
			filter.file = file;
		filter.line = line;

		//call
		return reader->profile->getCallStackNextLevel(parentStackId, parentDepth, filter);
	});
}

/**********************************************************/
const char * malt_reader_get_stacks_mem(malt_reader_t * reader)
{
	return malt_reader_hanle_request(reader, "GET_STACKS_MEM", [reader]() {
		return reader->profile->getStacksMem();
	});
}

/**********************************************************/
const char * malt_reader_get_stack_info_on_function(malt_reader_t * reader, size_t thread_id)
{
	return malt_reader_hanle_request(reader, "GET_STACK_INFO_ON_FUNCTION", [reader, thread_id]() {
		return reader->profile->getStackInfoOnFunction(thread_id);
	});
}

/**********************************************************/
const char * malt_reader_get_call_tree(malt_reader_t * reader, ssize_t nodeId, ssize_t depth, ssize_t height, double min_cost, const char * func, const char * metric, bool is_ratio)
{
	return malt_reader_hanle_request(reader, "GET_CALL_TREE", [reader, nodeId, depth, height, min_cost, func, metric, is_ratio]() {
		return reader->profile->getCallTree(nodeId, depth, height, min_cost, func, metric, is_ratio);
	});
}

/**********************************************************/
bool malt_reader_is_source_file(malt_reader_t * reader, const char * path)
{
	return reader->profile->isSourceFile(path);
}

/**********************************************************/
std::string malt_reader_json_request_cpp(malt_reader_t * reader, const std::string & request, bool * continue_run)
{
	//response
	std::string response;
	nlohmann::json responseJson;

	//parse request
	try {
		nlohmann::json reqJson = nlohmann::json::parse(request);
		fprintf(stderr, "[MALT-WEBVIEW-2] Request %s(%p)\n", reqJson.dump().c_str(), reader);
	
		//  Do some 'work'
		if (reqJson["operation"] == "getSummaryV2") {
			responseJson = reader->profile->getSummaryV2();
		} else if (reqJson["operation"] == "getSummary") {
			responseJson = reader->profile->getSummary();
		} else if (reqJson["operation"] == "getFlatFunctionProfile") {
			const bool own = reqJson["own"];
			const bool total = reqJson["total"];
			responseJson = reader->profile->getFlatFunctionProfile(own, total);
		} else if (reqJson["operation"] == "getFilterdStacksOnFileLine") {
			const std::string file = reqJson["file"];
			const size_t line = reqJson["line"];
			responseJson = reader->profile->getFilterdStacksOnFileLine(file, line);
		} else if (reqJson["operation"] == "getFilterdStacksOnSymbol") {
			const std::string func = reqJson["func"];
			responseJson = reader->profile->getFilterdStacksOnSymbol(func);
		} else if (reqJson["operation"] == "waitReady") {
			responseJson = nlohmann::json({"ready", true});
		} else if (reqJson["operation"] == "stop") {
			responseJson = nlohmann::json({"ready", true});
			if (continue_run)
				*continue_run = false;
		} else if (reqJson["operation"] == "getCallStackNextLevel") {
			const size_t parentStackId = reqJson["parentStackId"];
			const size_t parentStackLevel = reqJson["parentStackLevel"];
			LocationFilter filter;
			filter.function = reqJson["filter"]["function"];
			filter.file = reqJson["filter"]["file"];
			filter.line = reqJson["filter"]["line"];
			responseJson = reader->profile->getCallStackNextLevel(parentStackId, parentStackLevel, filter);
		} else if (reqJson["operation"] == "getFileLinesFlatProfile") {
			const std::string file = reqJson["file"];
			const bool total = reqJson["total"];
			responseJson = reader->profile->getFileLinesFlatProfile(file, total);
		} else if (reqJson["operation"] == "getTimedValues") {
			responseJson = reader->profile->getTimedValues();
		} else if (reqJson["operation"] == "getStacksMem") {
			responseJson = reader->profile->getStacksMem();
		} else if (reqJson["operation"] == "getStackInfoOnFunction") {
			const size_t thread_id = reqJson["thread_id"];
			responseJson = reader->profile->getStackInfoOnFunction(thread_id);
		} else if (reqJson["operation"] == "getSizeMap") {
			responseJson = reader->profile->getSizeMap();
		} else if (reqJson["operation"] == "getScatter") {
			responseJson = reader->profile->getScatter();
		} else if (reqJson["operation"] == "getReallocMap") {
			responseJson = reader->profile->getReallocMap();
		} else if (reqJson["operation"] == "getGlobalVariables") {
			responseJson = reader->profile->getGlobalVariables();
		} else if (reqJson["operation"] == "getCallTree") {
			size_t nodeid = reqJson["nodeid"];
			size_t depth = reqJson["depth"];
			size_t height = reqJson["height"];
			size_t mincost = reqJson["mincost"];
			std::string func = reqJson["func"];
			std::string metric = reqJson["metric"];
			bool isratio = reqJson["isratio"];
			std::string format = reqJson["format"];
			responseJson = reader->profile->getCallTree(nodeid, depth, height, mincost, func, metric, isratio);
		} else {
			responseJson = nlohmann::json({"error", "Invalid operation !"});
		}
	} catch (std::exception & e) {
		responseJson =  nlohmann::json({"error", "Invalid request or error during treatment : " + std::string(e.what())});
	}

	//convert
	response = responseJson.dump();
	responseJson = nlohmann::json();

	//ok
	return response;
}

/**********************************************************/
const char * malt_reader_json_request(malt_reader_t * reader, const char * request, bool * continue_run)
{
	malt_reader_free_last_response(reader);
	std::string str = malt_reader_json_request_cpp(reader, request, continue_run);
	const char * res = strdup(str.c_str());
	reader->last_response = res;
	//fprintf(stderr, "[MALT-WEBVIEW-2] Response : %s\n", res);
	return res;
}

/**********************************************************/
void malt_reader_free_response(const char * response)
{
	fprintf(stderr, "[MALT-WEBVIEW-2] free(%p)\n", response);
	free((void*)response);
}

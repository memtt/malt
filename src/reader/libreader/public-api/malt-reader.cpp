/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 10/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/public-api/malt-reader.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <cstdio>
#include <functional>
#include <nlohmann/json.hpp>
#include <iostream>
#include "../api/WebProfile.hpp"
#include "malt-reader.h"
#include "libinstrum/portability/Visibility.hpp"

/**********************************************************/
using namespace MALTReader;

/**********************************************************/
struct malt_reader_t
{
	WebProfile * profile{nullptr};
	bool verbose{false};
};

/**********************************************************/
extern "C"
{

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
			fprintf(stderr, "[MALT-WEBVIEW2] Loading OK => %p\n", reader);
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

/**********************************************************/
DLL_PUBLIC char * malt_reader_handle_request(struct malt_reader_t* reader, const char * name, std::function<nlohmann::json()> handler)
{
	try {
		if (reader->verbose)
			fprintf(stderr, "[MALT-WEBVIEW-2] Request %s(%p)\n", name, reader);
		nlohmann::json json = handler();
		std::string str = json.dump();
		char * res = strdup(str.c_str());
		//fprintf(stderr, "[MALT-WEBVIEW-2] Response : %s\n", res);
		return res;
	} catch (std::exception & e) {
		return nullptr;
	}
}

/**********************************************************/
DLL_PUBLIC char * malt_reader_json_get_proc_map(struct malt_reader_t* reader)
{
	return malt_reader_handle_request(reader, "GET_PROC_MAP", [reader]() {
		return reader->profile->getProcMap();
	});
}

/**********************************************************/
DLL_PUBLIC char * malt_reader_json_get_debug_stack_list(struct malt_reader_t* reader)
{
	return malt_reader_handle_request(reader, "GET_DEBUG_STACK_LIST", [reader]() {
		return reader->profile->getDebugStackList();
	});
}

/**********************************************************/
DLL_PUBLIC char * malt_reader_json_get_global_variables(struct malt_reader_t* reader)
{
	return malt_reader_handle_request(reader, "GEt_GLOBAL_VARIABLES", [reader]() {
		return reader->profile->getGlobalVariables();
	});
}

/**********************************************************/
DLL_PUBLIC char * malt_reader_json_get_summary(struct malt_reader_t* reader)
{
	return malt_reader_handle_request(reader, "GET_SUMMARY", [reader]() {
		return reader->profile->getSummary();
	});
}

/**********************************************************/
DLL_PUBLIC char * malt_reader_json_get_summary_v2(struct malt_reader_t* reader)
{
	return malt_reader_handle_request(reader, "GET_SUMMARY_V2", [reader]() {
		return reader->profile->getSummaryV2();
	});
}

/**********************************************************/
DLL_PUBLIC char * malt_reader_json_get_file_lines_flat_profile(struct malt_reader_t* reader, const char * file, bool total)
{
	return malt_reader_handle_request(reader, "GET_FILE_LINES_FLAT_PROFILE", [reader, file, total]() {
		return reader->profile->getFileLinesFlatProfile(file, total);
	});
}

/**********************************************************/
DLL_PUBLIC char * malt_reader_json_get_flat_function_profile(struct malt_reader_t* reader, bool own, bool total)
{
	return malt_reader_handle_request(reader, "GET_FLAT_FUNCTION_PROFILE", [reader, own, total]() {
		return reader->profile->getFlatFunctionProfile(own, total);
	});
}

/**********************************************************/
DLL_PUBLIC char * malt_reader_json_get_proc_map_distr(struct malt_reader_t* reader)
{
	return malt_reader_handle_request(reader, "GET_PROC_MAP_DISTR", [reader]() {
		return reader->profile->getProcMapDistr();
	});
}

/**********************************************************/
DLL_PUBLIC char * malt_reader_json_get_scatter(struct malt_reader_t* reader)
{
	return malt_reader_handle_request(reader, "GET_SCATTER", [reader]() {
		return reader->profile->getScatter();
	});
}

/**********************************************************/
DLL_PUBLIC char * malt_reader_json_get_size_map(struct malt_reader_t* reader)
{
	return malt_reader_handle_request(reader, "GET_SIZE_MAP", [reader]() {
		return reader->profile->getSizeMap();
	});
}

/**********************************************************/
DLL_PUBLIC char * malt_reader_json_get_realloc_map(struct malt_reader_t* reader)
{
	return malt_reader_handle_request(reader, "GET_REALLOC_MAP", [reader]() {
		return reader->profile->getReallocMap();
	});
}

/**********************************************************/
DLL_PUBLIC char * malt_reader_json_get_timed_values(struct malt_reader_t* reader)
{
	return malt_reader_handle_request(reader, "GET_TIMED_VALUES", [reader]() {
		return reader->profile->getTimedValues();
	});
}

/**********************************************************/
DLL_PUBLIC char * malt_reader_json_get_filtered_stacks_on_file_line(struct malt_reader_t* reader, const char * file, size_t line)
{
	return malt_reader_handle_request(reader, "GET_FILTERED_STACKS_ON_FILE_LINE", [reader, file, line]() {
		return reader->profile->getFileLinesFlatProfile(file, line);
	});
}

/**********************************************************/
DLL_PUBLIC char * malt_reader_json_get_filterd_stacks_on_symbol(struct malt_reader_t *reader, const char * func)
{
	return malt_reader_handle_request(reader, "GET_FILTERED_STACKS_ON_SYMBOL", [reader, func]() {
		return reader->profile->getFilterdStacksOnSymbol(func);
	});
}

/**********************************************************/
DLL_PUBLIC char * malt_reader_json_get_binary_addresses_profile(struct malt_reader_t* reader, const char * binary, void ** addresses, size_t addr_count, bool total)
{
	//build array
	std::vector<size_t> addr;
	addr.reserve(addr_count);
	for (size_t i = 0; i < addr_count ; i++)
		addr.push_back((size_t)addresses[i]);

	//call
	return malt_reader_handle_request(reader, "GET_BINARY_ADDRESSSES_PROFILE", [reader, binary, addr, addr_count, total]() {
		return reader->profile->getBinaryAddressesFlatProfile(binary, addr, total);
	});
}

/**********************************************************/
DLL_PUBLIC char * malt_reader_json_get_binary_addresses_profile_all(struct malt_reader_t* reader, const char * binary, bool total)
{
	//call
	return malt_reader_handle_request(reader, "GET_BINARY_ADDRESSSES_PROFILE", [reader, binary, total]() {
		return reader->profile->getBinaryAddressesFlatProfileAll(binary, total);
	});
}

/**********************************************************/
DLL_PUBLIC char * malt_reader_json_get_call_stack_next_level(struct malt_reader_t* reader, size_t parentStackId, size_t parentDepth, const char * function, const char * file, int line)
{
	return malt_reader_handle_request(reader, "GET_CALL_STACK_NEXT_LEVEL", [reader, parentStackId, parentDepth, file, function, line]() {
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
DLL_PUBLIC char * malt_reader_json_get_stacks_mem(struct malt_reader_t* reader)
{
	return malt_reader_handle_request(reader, "GET_STACKS_MEM", [reader]() {
		return reader->profile->getStacksMem();
	});
}

/**********************************************************/
DLL_PUBLIC char * malt_reader_json_get_stack_info_on_function(struct malt_reader_t* reader, size_t thread_id)
{
	return malt_reader_handle_request(reader, "GET_STACK_INFO_ON_FUNCTION", [reader, thread_id]() {
		return reader->profile->getStackInfoOnFunction(thread_id);
	});
}

/**********************************************************/
DLL_PUBLIC char * malt_reader_json_get_call_tree(struct malt_reader_t* reader, ssize_t nodeId, ssize_t depth, ssize_t height, double min_cost, const char * func, const char * metric, bool is_ratio)
{
	return malt_reader_handle_request(reader, "GET_CALL_TREE", [reader, nodeId, depth, height, min_cost, func, metric, is_ratio]() {
		return reader->profile->getCallTree(nodeId, depth, height, min_cost, func, metric, is_ratio);
	});
}

/**********************************************************/
DLL_PUBLIC bool malt_reader_is_source_file(struct malt_reader_t* reader, const char * path)
{
	return reader->profile->isSourceFile(path);
}

/**********************************************************/
DLL_PUBLIC char * malt_reader_json_request(struct malt_reader_t* reader, const char * request, bool * continue_run)
{
	std::string str = malt_reader_json_request_cpp(reader, request, continue_run);
	char * res = strdup(str.c_str());
	//fprintf(stderr, "[MALT-WEBVIEW-2] Response : %s\n", res);
	return res;
}

}

/**********************************************************/
DLL_PUBLIC std::string malt_reader_json_request_cpp(struct malt_reader_t* reader, const std::string & request, bool * continue_run)
{
	//response
	std::string response;
	nlohmann::json responseJson;

	//parse request
	try {
		nlohmann::json reqJson = nlohmann::json::parse(request);
		if (reader->verbose)
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
		} else if (reqJson["operation"] == "getBinaryAddressesProfile") {
			std::string binary = reqJson["binary"];
			std::vector<size_t> addresses;
			reqJson["addresses"].get_to(addresses);
			bool total = reqJson["total"];
			responseJson = reader->profile->getBinaryAddressesFlatProfile(binary, addresses, total);
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

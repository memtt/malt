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
#include <iostream>
#include <httplib.h>
#include <vector>
#include "lib/BasicAuth.hpp"
#include "api/WebProfile.hpp"
#include "extractors/ExtractorHelpers.hpp"

/**********************************************************/
using namespace httplib;
using namespace MALTWebviewCpp;
using namespace MALTReader;

/**********************************************************/
//https://stackoverflow.com/questions/116038/how-do-i-read-an-entire-file-into-a-stdstring-in-c
std::string load_full_file(const std::string &fileName)
{
	std::ifstream ifs(fileName.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

	std::ifstream::pos_type fileSize = ifs.tellg();
	ifs.seekg(0, std::ios::beg);

	char * buffer = new char[fileSize];
	ifs.read(buffer, fileSize);

	return std::string(buffer, fileSize);
}

/**********************************************************/
int main(int argc, char ** argv)
{
	//loading profile
	WebProfile profile(argv[1], true);

	//spwn server
	BasicAuth basicAuth("malt-webview");
	Server svr;

	svr.set_pre_routing_handler([&basicAuth](const Request& req, Response& res) {
		return basicAuth.check(req, res);
	});

	svr.set_logger([](const Request& req, const Response& res) {
		printf("[%s] %s => (%d)\n", req.method.c_str(), req.path.c_str(), res.status);
	});

	svr.Get("/", [](const Request& req, Response& res) {
		res.set_redirect("app/index.html");
	});

	svr.set_mount_point("/app", "./client-files/app");

	svr.Get("/summary.json", [&profile](const Request& req, Response& res) {
		nlohmann::json data = profile.getSummary();
		res.set_content(data.dump(), "application/json");
	});

	svr.Get("/data/summary.json", [&profile](const Request& req, Response& res) {
		nlohmann::json data = profile.getSummaryV2();
		res.set_content(data.dump(), "application/json");
	});

	svr.Get("/flat.json", [&profile](const Request& req, Response& res) {
		nlohmann::json data = profile.getFlatFunctionProfile(true, true);
		res.set_content(data.dump(), "application/json");
	});

	svr.Post("/stacks.json", [&profile](const Request& req, Response& res) {
		nlohmann::json data;
		if (req.has_param("file") && req.has_param("line")) {
			data = profile.getFilterdStacksOnFileLine(req.get_param_value("file"), std::stoi(req.get_param_value("line")));
		} else {
			data = profile.getFilterdStacksOnSymbol(req.get_param_value("func"));
		}
		res.set_content(data.dump(), "application/json");
	});

	svr.Post("/source-file", [&profile](const Request& req, Response& res) {
		#warning "use the check function"
		#warning "check parameters going in"
		const nlohmann::json json = nlohmann::json::parse(req.body);
		const std::string path = json["path"];
		const std::string data = load_full_file(path);
		res.set_content(data, "plain/text");
	});

	svr.Post("/file-infos.json", [&profile](const Request& req, Response& res) {
		#warning "check parameters going in"
		const nlohmann::json json = nlohmann::json::parse(req.body);
		const std::string file = json["file"];
		const std::string data = profile.getFileLinesFlatProfile(file, true);
		res.set_content(data, "application/json");
	});

	svr.Post("/call-stack-next-level.json", [&profile](const Request& req, Response& res) {
		#warning "check parameters going in"
		const nlohmann::json json = nlohmann::json::parse(req.body);
		LocationFilter filter;
		filter.function = json["filter"].value("function", "");
		filter.file = json["filter"].value("file", "");
		filter.line = json["filter"].value("line", -1);
		const size_t parentStackId = json.value("parentStackId", 0);
		const size_t parentStackDepth = json.value("parentStackDepth", 0);
		const nlohmann::json data = profile.getCallStackNextLevel(parentStackId, parentStackDepth, filter);
		res.set_content(data.dump(), "application/json");
	});

	svr.Get("/timed.json", [&profile](const Request& req, Response& res) {
		nlohmann::json data = profile.getTimedValues();
		res.set_content(data.dump(), "application/json");
	});

	svr.Get("/stacks-mem.json", [&profile](const Request& req, Response& res) {
		nlohmann::json data = profile.getStacksMem();
		res.set_content(data.dump(), "application/json");
	});

	svr.Get("/stack.json", [&profile](const Request& req, Response& res) {
		const size_t id = stoi(req.get_param_value("id"));
		nlohmann::json data = profile.getStackInfoOnFunction(id);
		res.set_content(data.dump(), "application/json");
	});

	svr.Get("/size-map.json", [&profile](const Request& req, Response& res) {
		nlohmann::json data = profile.getSizeMap();
		res.set_content(data.dump(), "application/json");
	});

	svr.Get("/scatter.json", [&profile](const Request& req, Response& res) {
		nlohmann::json data = profile.getScatter();
		res.set_content(data.dump(), "application/json");
	});

	svr.Get("/realloc-map.json", [&profile](const Request& req, Response& res) {
		nlohmann::json data = profile.getReallocMap();
		res.set_content(data.dump(), "application/json");
	});

	svr.Get("/global-variables.json", [&profile](const Request& req, Response& res) {
		nlohmann::json data = profile.getGlobalVariables();
		res.set_content(data.dump(), "application/json");
	});

	svr.Post("/calltree", [&profile](const Request& req, Response& res) {
		const nlohmann::json json = nlohmann::json::parse(req.body);
		std::cerr << json << std::endl;
		const std::string format = json.value("format", "");
		const std::string func = ""; //json.value("func", "");
		const ssize_t nodeid = json.value("nodeid", -1);
		const ssize_t depth = json.value("depth", -1);
		const ssize_t height = json.value("height", -1);
		const ssize_t mincost = json.value("mincost", 0.0);
		const std::string metric = json.value("metric", "");
		const bool isratio = json.value("isratio", false);
		nlohmann::json data = profile.getCallTree(nodeid, depth, height, mincost, func, metric, isratio);
		res.set_content(data.dump(), "application/json");
	});

	//listen
	printf("Listening on http://localhost:8080\n");
	svr.listen("localhost", 8080);
	
	return EXIT_SUCCESS;
}

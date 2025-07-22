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
#include <cstdlib>
#include <iostream>
#include <httplib.h>
#include <vector>
#include <fstream>
#include "lib/BasicAuth.hpp"
#include "api/WebProfile.hpp"
#include "extractors/ExtractorHelpers.hpp"
//gnu libc
#include <argp.h>
#include <libgen.h>

/**********************************************************/
using namespace httplib;
using namespace MALTWebviewCpp;
using namespace MALTReader;

/**********************************************************/
const char *argp_program_version = "MALT 1.2.4";
const char *argp_program_bug_address = "<sebastien.valat.dev@orange.fr>";

/**********************************************************/
struct WebviewOptions
{
	WebviewOptions(void);
	void parse(int argc, char ** argv);
	std::string filename{};
	bool auth{true};
	uint32_t port{8080};
	std::string socket{};
};

/**********************************************************/
WebviewOptions::WebviewOptions()
{

}

/**********************************************************/
static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
	/* Get the input argument from argp_parse, which we
		know is a pointer to our arguments structure. */
	WebviewOptions *options = static_cast<WebviewOptions*>(state->input);

	//cases
	switch (key) {
		case 'n':
			options->auth = false;
			break;
		case 'p':
			options->port = atoi(arg);
			break;
		case 's':
			options->socket = arg;
			break;
		case ARGP_KEY_ARG:
			/* Too many arguments. */
			if (state->arg_num >= 1)
				argp_usage (state);
			options->filename = arg;
			break;
		case ARGP_KEY_END:
			/* Not enough arguments. */
			if (state->arg_num < 1)
				argp_usage (state);
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

/**********************************************************/
void WebviewOptions::parse(int argc, char ** argv)
{
	char doc[] = "malt-webview-new -- Micro-server to expose the MALT webview and brower into the MALT profile.";
	char args_doc[] = "[--no-auth] [-p 8080] [-s UNIX_SOCKET] {PROFILE_FILE}";
	struct argp_option options[] = {
		{"no-auth",    'n', 0,        0,  "Disable the authentication." },
		{"port",       'p', "PORT",   0,  "Listening on given port (8080 by default)."},
		{"socket",     's', "FILE",   0,  "Listening on given unix socket."},
		{ 0 }
	};
	struct argp argp = { options, parse_opt, args_doc, doc };
	argp_parse(&argp, argc, argv, 0, 0, this);
}

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
static std::string get_current_exe(void)
{
	char buffer[4096] = {0};
	const ssize_t res = readlink("/proc/self/exe", buffer, sizeof(buffer));
	buffer[res] = '\0';
	return buffer;
}

/**********************************************************/
static std::string get_webview_www_path(void)
{
	const std::string exe_path = get_current_exe();
	const std::string bin_path = dirname((char*)exe_path.c_str());
	const std::string prefix = dirname((char*)bin_path.c_str());
	return prefix + std::string("/share/malt/webview");
}

/**********************************************************/
int main(int argc, char ** argv)
{
	//parse options
	WebviewOptions options;
	options.parse(argc, argv);

	//print
	printf("Loading %s...\n", options.filename.c_str());

	//loading profile
	WebProfile profile(options.filename, true);

	//spwn server
	BasicAuth basicAuth("malt-webview");
	Server svr;

	//if auth is enabled
	if (options.auth) {
		svr.set_pre_routing_handler([&basicAuth](const Request& req, Response& res) {
			return basicAuth.check(req, res);
		});
	}

	//enable logging
	svr.set_logger([](const Request& req, const Response& res) {
		printf("[%s] %s => (%d)\n", req.method.c_str(), req.path.c_str(), res.status);
	});

	//handle home page
	svr.Get("/", [](const Request& req, Response& res) {
		res.set_redirect("app/index.html");
	});

	//mount app path
	const std::string www_path = get_webview_www_path();
	svr.set_mount_point("/app", www_path + "/client-files/app");

	//data for the summary
	svr.Get("/summary.json", [&profile](const Request& req, Response& res) {
		nlohmann::json data = profile.getSummary();
		res.set_content(data.dump(), "application/json");
	});

	//new version of the summary
	svr.Get("/data/summary.json", [&profile](const Request& req, Response& res) {
		nlohmann::json data = profile.getSummaryV2();
		res.set_content(data.dump(), "application/json");
	});

	//flat profile
	svr.Get("/flat.json", [&profile](const Request& req, Response& res) {
		nlohmann::json data = profile.getFlatFunctionProfile(true, true);
		res.set_content(data.dump(), "application/json");
	});

	//call stack tree
	svr.Post("/stacks.json", [&profile](const Request& req, Response& res) {
		nlohmann::json data;
		if (req.has_param("file") && req.has_param("line")) {
			data = profile.getFilterdStacksOnFileLine(req.get_param_value("file"), std::stoi(req.get_param_value("line")));
		} else {
			data = profile.getFilterdStacksOnSymbol(req.get_param_value("func"));
		}
		res.set_content(data.dump(), "application/json");
	});

	//source file
	svr.Post("/source-file", [&profile](const Request& req, Response& res) {
		#warning "use the check function"
		#warning "check parameters going in"
		const nlohmann::json json = nlohmann::json::parse(req.body);
		const std::string path = json["path"];
		const std::string data = load_full_file(path);
		res.set_content(data, "plain/text");
	});

	//infos of the file
	svr.Post("/file-infos.json", [&profile](const Request& req, Response& res) {
		#warning "check parameters going in"
		const nlohmann::json json = nlohmann::json::parse(req.body);
		const std::string file = json["file"];
		const std::string data = profile.getFileLinesFlatProfile(file, true);
		res.set_content(data, "application/json");
	});

	//dig into the call stack tree
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

	//for time charts
	svr.Get("/timed.json", [&profile](const Request& req, Response& res) {
		nlohmann::json data = profile.getTimedValues();
		res.set_content(data.dump(), "application/json");
	});

	//memory in stack of threads
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

	//print
	printf("+-------------------------------------------------------------------------+\n");
	printf("|                                                                         |\n");
	printf("|                    _ _                    _          _                  |\n");
	printf("|    _ __ ___   __ _| | |_    __      _____| |____   _(_) _____      __   |\n");
	printf("|   | '_ ` _ \\ / _` | | __|___\\ \\ /\\ / / _ \\ '_ \\ \\ / / |/ _ \\ \\ /\\ / /   |\n");
	printf("|   | | | | | | (_| | | ||_____\\ V  V /  __/ |_) \\ V /| |  __/\\ V  V /    |\n");
	printf("|   |_| |_| |_|\\__,_|_|\\__|     \\_/\\_/ \\___|_.__/ \\_/ |_|\\___| \\_/\\_/     |\n");
	printf("|                                                                         |\n");
	printf("|                                                                         |\n");
	if (options.socket.empty()) {
		printf("|         Starting server listening on http://localhost:%-4d/             |\n", options.port);
	}else {
		std::string value = std::string("unix://") + options.socket;
		int left_padding = (71 - value.size()) / 2;
		int right_padding = 71 - value.size() - left_padding;
		printf("|                        Starting server listening on                     |\n");
		printf("| %*s%s%*s |\n", left_padding, " ", value.c_str(), right_padding, " ");
	}
	printf("|                                                                         |\n");
	printf("|                        To use from remote you can :                     |\n");
	printf("|                 user> ssh -L8080:localhost:8080 myserver                |\n");
	printf("|                                                                         |\n");
	printf("+-------------------------------------------------------------------------+\n");

	//listen
	if (options.socket.empty()) {
		svr.listen("localhost", 8080);
	} else {
		unlink(options.socket.c_str());
		svr.set_address_family(AF_UNIX).listen(options.socket, 8080);
	}
	
	return EXIT_SUCCESS;
}

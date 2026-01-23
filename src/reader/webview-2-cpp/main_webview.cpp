/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/webview-2-cpp/main_webview.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
*    AUTHOR   : Sébastien Valat - 2026
***********************************************************/

/**********************************************************/
//std C
#include <cstdlib>
//std C++
#include <iostream>
#include <fstream>
#include <filesystem>
//cpp-httplib
#include <httplib.h>
//unix
#include <signal.h>
//gnu libc
#include <argp.h>
#include <libgen.h>
//internal
#include "lib/TokenAuth.hpp"
#include "lib/ArgChecker.hpp"
#include "api/WebProfile.hpp"
#include "extractors/ExtractorHelpers.hpp"

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
	std::list<std::string> overrides;
	std::string host{"localhost"};
	bool regenToken{false};
	std::string staticGen{""};
	bool staticSummary{false};
	bool compressProfile{false};
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
		case 'p': {
			int port = atoi(arg);
			if (port == 0)
				options->socket = arg;
			else
				options->port = port;
			break;
		}
		case 'o':
			options->overrides.push_back(arg);
			break;
		case 'H':
			options->host = arg;
			break;
		case 'r':
			options->regenToken = true;
			break;
		case 's':
			options->staticGen = arg;
			options->staticSummary = false;
			break;
		case 'S':
			options->staticGen = arg;
			options->staticSummary = true;
			break;
		case 'x':
			options->compressProfile = true;
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
	char args_doc[] = "[--no-auth] [-H localhost] [-p 8080] [-s UNIX_SOCKET] {PROFILE_FILE}";
	struct argp_option options[] = {
		{"no-auth",        'n', 0,        0,  "Disable the authentication." },
		{"port",           'p', "PORT",   0,  "Listening on given port or socket file (8080 by default)."},
		{"override",       'o', "OLD:NEW",0,  "Override some source path by the given path, in the form : OLD:NEW. Can be called several times."},
		{"host",           'H', "HOST",   0,  "The host interface to listen on (localhost by default)."},
		{"regen-token",    'r', 0,        0,  "Regenerate the token."},
		{"xz",             'x', 0,        0,  "In conjunction with --static or --static-summary, compress the profile with xz."},
		{"static",         's', "DIR",    0,  "Generate a static version of the website in the given directory."},
		{"static-summary", 'S', "DIR",    0,  "Generate a static version of the website in the given directory with only the summary."},
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

	if (ifs.fail())
		return "";

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
	const std::string webview = prefix + std::string("/share/malt/webview/dynamic/");
	const std::string webviewCheckFile = webview + std::string("index.html");
	FILE * fp = fopen(webviewCheckFile.c_str(), "r");
	if (fp == nullptr) {
		return std::string(MALT_INSTALL_PREFIX) + "/share/malt/webview/dynamic/";
	} else {
		fclose(fp);
		return webview;
	}
}

/**********************************************************/
static std::string get_webview_www_static_path(bool onlySummary = false)
{
	//select mode
	std::string partName = "static";
	if (onlySummary)
		partName = "summary";

	//build paths
	const std::string exe_path = get_current_exe();
	const std::string bin_path = dirname((char*)exe_path.c_str());
	const std::string prefix = dirname((char*)bin_path.c_str());
	const std::string webview = prefix + std::string("/share/malt/webview/") + partName;
	const std::string webviewCheckFile = webview + std::string("index.html");
	FILE * fp = fopen(webviewCheckFile.c_str(), "r");
	if (fp == nullptr) {
		return std::string(MALT_INSTALL_PREFIX) + std::string("/share/malt/webview/") + partName;
	} else {
		fclose(fp);
		return webview;
	}
}

/**********************************************************/
Server * gblServerPtr = nullptr;
static void local_signal_ctrl_c_handler(int s){
	printf("Stopping server...\n");
	gblServerPtr->stop();
}

/**********************************************************/
std::string protectShellString(const std::string & value)
{
	std::stringstream out;
	out << '"';
	for (const auto it : value) {
		if (it == '"')
			out << '\\';
		out << it;
	}
	out << '"';
	return out.str();
}

/**********************************************************/
std::string genCompressCommand(const std::string & in, const std::string & out)
{
	//set
	const std::string in_protected = protectShellString(in);
	const std::string out_protected = protectShellString(out);

	//build
	std::stringstream cmd;
	cmd << "cat " << in_protected << " | xz -T0  > " << out_protected;
	return cmd.str();
}

/**********************************************************/
bool genStaticWebsite(const WebProfile & profile, const std::string & path, bool onlySummary = false, bool compressProfile = false)
{
	//remove
	std::filesystem::remove(path + "/index.html");
	std::filesystem::remove(path + "/favicon.ico");
	std::filesystem::remove_all(path + "/data");

	//create the directory
	if (std::filesystem::exists(path + "/data") == false) {
		const bool status = std::filesystem::create_directories(path + "/data");
		if (!status) {
			std::cerr << "Fail to create the directory : " << path << std::endl;
			return false;
		}
	}

	//target profile name
	std::string profileName = std::filesystem::path(profile.getFileName()).filename();
	if (compressProfile)
		profileName += ".xz";

	//create data file
	const std::string dataFName = path + "/data/static-profile.js";
	std::ofstream dataOut(dataFName);
	if (onlySummary) {
		dataOut << "const MALT_DATA = " << profile.getStaticSummary() << ";" << std::endl;
	} else {
		dataOut << "const MALT_DATA = " << profile.getStatic() << ";" << std::endl;
	}
	dataOut << "const MALT_PROFILE_PATH = \"data/" << profileName << "\";" << std::endl;
	dataOut.close();

	//copy static html
	std::filesystem::copy_file(get_webview_www_static_path(onlySummary) + "/index.html", path + "/index.html");
	std::filesystem::copy_file(get_webview_www_static_path(onlySummary) + "/favicon.ico", path + "/favicon.ico");

	//xz compress the profile
	if (compressProfile) {
		const std::string xzCmd = genCompressCommand(profile.getFileName(), path + "/data/" + profileName);
		int statusSystem = std::system(xzCmd.c_str());
		if (statusSystem != 0)
			throw std::runtime_error("Fail to compress the profile file !");
	} else {
		std::filesystem::copy_file(profile.getFileName(), path + "/data/" + profileName);
	}

	//ok
	return true;
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
	try {
		//load profile
		WebProfile profile(options.filename, true);

		//if statis, trivial
		if (options.staticGen.empty() == false) {
			bool status = genStaticWebsite(profile, options.staticGen, options.staticSummary, options.compressProfile);
			if (status) {
				return EXIT_SUCCESS;
			} else {
				std::cerr << "Fail to generate the static website !" << std::endl;
				return EXIT_FAILURE;
			}
		}

		//spwn server
		TokenAuth * tokenAuth = nullptr;
		Server svr;
		gblServerPtr = &svr;

		//arg checker
		ArgChecker argChecker;
		argChecker.allowPaths(profile.getSourceFileMap());
		argChecker.overridePaths(options.overrides);

		//set signal handler
		signal(SIGINT, local_signal_ctrl_c_handler);

		//no auth files
		std::set<std::string> noAuthFiles;
		noAuthFiles.insert("/");
		noAuthFiles.insert("/home");
		noAuthFiles.insert("/sources");
		noAuthFiles.insert("/call-tree");
		noAuthFiles.insert("/timeline");
		noAuthFiles.insert("/allocSizeDistr");
		noAuthFiles.insert("/globalVars");
		noAuthFiles.insert("/stackPeaks");
		noAuthFiles.insert("/per-thread");
		noAuthFiles.insert("/realloc");
		noAuthFiles.insert("/index.html");
		noAuthFiles.insert("/favicon.ico");
		noAuthFiles.insert("/assets/index.js");
		noAuthFiles.insert("/assets/style.css");
		noAuthFiles.insert("/assets/MavenPro-VariableFont_wght.ttf");

		//if auth is enabled
		if (options.auth) {
			tokenAuth = new TokenAuth(options.regenToken);
			svr.set_pre_routing_handler([tokenAuth, &noAuthFiles](const Request& req, Response& res) {
				//we let access to the app index.html/js/css
				if (noAuthFiles.find(req.path) != noAuthFiles.end())
					return Server::HandlerResponse::Unhandled;

				//secrure all the rest
				return tokenAuth->check(req, res);
			});
		}

		//enable logging
		svr.set_logger([](const Request& req, const Response& res) {
			printf("[%s] %s => (%d)\n", req.method.c_str(), req.path.c_str(), res.status);
		});

		//handle home page
		svr.Get("/", [](const Request& req, Response& res) {
			res.set_redirect("index.html");
		});

		//mount app path
		const std::string www_path = get_webview_www_path();
		svr.set_mount_point("/", www_path);

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
		svr.Post("/source-file", [&profile, &argChecker](const Request& req, Response& res) {
			const std::string path = argChecker.checkJsonArgPath(req, "path");
			const std::string data = load_full_file(path);
			res.set_content(data, "plain/text");
		});

		//infos of the file
		svr.Post("/file-infos.json", [&profile, &argChecker](const Request& req, Response& res) {
			const std::string file = argChecker.checkJsonArgPath(req, "file");
			const nlohmann::json data = profile.getFileLinesFlatProfile(file, true);
			res.set_content(data.dump(), "application/json");
		});

		//dig into the call stack tree
		svr.Post("/call-stack-next-level.json", [&profile](const Request& req, Response& res) {
			//#warning "check parameters going in"
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
			std::string func = "";
			if (json.contains("func") && json["func"].is_null() == false)
				func = json.value("func", "");
			ssize_t nodeId = -1;
			if (json.contains("nodeid") && json["nodeid"].is_null() == false)
				nodeId = json.value("nodeid", -1L);
			const ssize_t depth = json.value("depth", -1);
			const ssize_t height = json.value("height", -1);
			const ssize_t mincost = json.value("mincost", 0.0);
			const std::string metric = json.value("metric", "");
			const bool isratio = json.value("isratio", false);
			nlohmann::json data = profile.getCallTree(nodeId, depth, height, mincost, func, metric, isratio);
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
			printf("|         Starting server listening on http://%s:%-4d/             |\n", options.host.c_str(), options.port);
		}else {
			std::string value = std::string("unix://") + options.socket;
			int left_padding = (71 - value.size()) / 2;
			int right_padding = 71 - value.size() - left_padding;
			printf("|                        Starting server listening on                     |\n");
			printf("| %*s%s%*s |\n", left_padding, " ", value.c_str(), right_padding, " ");
		}
		printf("|                                                                         |\n");
		printf("|                        To use from remote you can :                     |\n");
		if (options.socket.empty()) {
			printf("|               user> ssh -L8080:%s:%-4d myserver                  |\n", options.host.c_str(), options.port);
		}else {
			std::string value = std::string("user> ssh -L8080:") + options.host + std::string(":") + options.socket + (" myserver");
			int left_padding = (71 - value.size()) / 2;
			int right_padding = 71 - value.size() - left_padding;
			printf("|                        Starting server listening on                     |\n");
			printf("| %*s%s%*s |\n", left_padding, " ", value.c_str(), right_padding, " ");
		}
		if (tokenAuth != nullptr) {
			printf("|                                                                         |\n");
			const std::string token = tokenAuth->getToken();
			printf("|                          Authentication token                           |\n");
			printf("|             %s            |\n", token.c_str());
		}
		printf("|                                                                         |\n");
		printf("+-------------------------------------------------------------------------+\n");

		//listen
		if (options.socket.empty()) {
			svr.listen(options.host.c_str(), options.port);
		} else {
			svr.set_address_family(AF_UNIX).listen(options.socket.c_str(), 8080);
			unlink(options.socket.c_str());
		}

		//clean
		if (tokenAuth != nullptr)
			delete tokenAuth;
	} catch (std::runtime_error & error) {
		std::cerr << "ERROR : " << error.what() << std::endl;
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}

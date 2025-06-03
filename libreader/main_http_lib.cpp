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
#include "api/WebProfile.hpp"
#include "extractors/ExtractorHelpers.hpp"

/**********************************************************/
using namespace httplib;
using namespace MALTReader;

/**********************************************************/
int main(int argc, char ** argv)
{
	//loading profile
	WebProfile profile(argv[1], true);

	//spwn server
	Server svr;

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

	svr.Get("/flat.json", [&profile](const Request& req, Response& res) {
		nlohmann::json data = profile.getFlatFunctionProfile(true, true);
		res.set_content(data.dump(), "application/json");
	});

	//listen
	printf("Listening on http://localhost:8080\n");
	svr.listen("localhost", 8080);
	
	return EXIT_SUCCESS;
}

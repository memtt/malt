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
#include <zmq.hpp>
#include "WebProfile.hpp"
#include "extractors/ExtractorHelpers.hpp"

/**********************************************************/
using namespace MALTReader;

/**********************************************************/
int main(int argc, char ** argv)
{
	//  Prepare our context and socket
	zmq::context_t context (2);
	zmq::socket_t socket (context, zmq::socket_type::rep);
	socket.bind ("ipc:///tmp/socket");

	//loading profile
	WebProfile profile(argv[1], true);

	bool continueRun = true;
	while (continueRun) {
		zmq::message_t request;

		//  Wait for next request from client
		socket.recv (request, zmq::recv_flags::none);
		std::cout << "Received " << request.to_string() << std::endl;

		//response
		std::string response;
		nlohmann::json responseJson;

		//parse request
		try {
			nlohmann::json reqJson = nlohmann::json::parse(request.to_string());
			std::cout << reqJson << std::endl;
		
			//  Do some 'work'
			if (reqJson["operation"] == "getSummaryV2") {
				responseJson = profile.getSummaryV2();
			} else if (reqJson["operation"] == "getSummary") {
				responseJson = profile.getSummary();
			} else if (reqJson["operation"] == "getFlatFunctionProfile") {
				const bool own = reqJson["own"];
				const bool total = reqJson["total"];
				responseJson = profile.getFlatFunctionProfile(own, total);
			} else if (reqJson["operation"] == "getFilterdStacksOnFileLine") {
				const std::string file = reqJson["file"];
				const size_t line = reqJson["line"];
				responseJson = profile.getFilterdStacksOnFileLine(file, line);
			} else if (reqJson["operation"] == "getFilterdStacksOnSymbol") {
				const std::string func = reqJson["func"];
				responseJson = profile.getFilterdStacksOnSymbol(func);
			} else if (reqJson["operation"] == "waitReady") {
				responseJson = nlohmann::json({"ready", true});
			} else if (reqJson["operation"] == "stop") {
				responseJson = nlohmann::json({"ready", true});
				continueRun = false;
			} else if (reqJson["operation"] == "getCallStackNextLevel") {
				const size_t parentStackId = reqJson["parentStackId"];
				const size_t parentDepth = reqJson["parentDepth"];
				responseJson = profile.getCallStackNextLevel(parentStackId, parentDepth);
			} else {
				responseJson = nlohmann::json({"error", "Invalid operation !"});
			}
		} catch (std::exception & e) {
			responseJson =  nlohmann::json({"error", "Invalid request or error during treatment !"});
		}

		//convert
		response = responseJson.dump();
		responseJson = nlohmann::json();

		//  Send reply back to client
		zmq::message_t reply (response.c_str(), response.size());
		socket.send (reply, zmq::send_flags::none);
	}

	return EXIT_SUCCESS;
}

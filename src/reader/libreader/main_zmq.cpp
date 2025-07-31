/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 06/2025
*    LICENSE  : CeCILL-C
*    FILE     : ./src/reader/libreader/main_zmq.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <iostream>
#include <zmq.hpp>
#include "api/Requests.h"

/**********************************************************/
int main(int argc, char ** argv)
{
	//  Prepare our context and socket
	zmq::context_t context (2);
	zmq::socket_t socket (context, zmq::socket_type::rep);
	socket.bind ("ipc:///tmp/socket");

	//loading profile
	bool continueRun = true;
	malt_reader_t * reader = malt_reader_init(argv[1], true, false);

	//loop
	while (continueRun) {
		zmq::message_t request;

		//  Wait for next request from client
		socket.recv (request, zmq::recv_flags::none);

		//display
		std::string req_str = request.to_string();
		std::cout << "Received " << req_str << std::endl;

		//handle
		std::string response = malt_reader_json_request_cpp(reader, req_str, &continueRun);
		//std::cout << response << std::endl;

		//  Send reply back to client
		zmq::message_t reply (response.c_str(), response.size());
		socket.send (reply, zmq::send_flags::none);
	}

	//fini
	malt_reader_fini(reader);

	return EXIT_SUCCESS;
}

/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/main_stream.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
*    AUTHOR   : Sébastien Valat - 2026
***********************************************************/

/**********************************************************/
#include <iostream>
#include <nlohmann/json.hpp>
#include "public-api/malt-reader.h"

/**********************************************************/
int main(int argc, char ** argv)
{
	//loading profile
	bool continueRun = true;
	malt_reader_t * reader = malt_reader_init(argv[1], MALT_READER_PROGRESS_BAR);

	//loop
	while (std::cin && continueRun) {
		//load request
		nlohmann::json req_json;
		std::cin >> req_json;
		std::string req_str = req_json.dump();

		//handle
		std::string response = malt_reader_json_request_cpp(reader, req_str, &continueRun);

		//print
		std::cout << response << std::endl;
	}

	//fini
	malt_reader_fini(reader);

	//ok
	return EXIT_SUCCESS;
}

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
#include "api/WebProfile.hpp"
#include "extractors/ExtractorHelpers.hpp"

/**********************************************************/
using namespace MALTReader;

/**********************************************************/
int main(int argc, char ** argv)
{
	WebProfile profile(argv[1], true);
	//std::cout << profile.getFileLinesFlatProfile("/home/svalat/Projects/test-memray-scalene/venv/lib/python3.13/site-packages/matplotlib/backends/_backend_tk.py", false) << std::endl;
	std::cout << profile.getSummary() << std::endl;

	return EXIT_SUCCESS;


	const Extractor & extractor = profile.getExtractor();

	//extract
	fprintf(stderr, "Treat\n");
	FlatProfileVector res = extractor.getFlatProfile([](const InstructionInfosStrRef & location, const MALTFormat::StackInfos & infos){
		return *location.function;
	},[](const InstructionInfosStrRef & location, const MALTFormat::StackInfos & infos){
		return true;
	});

	fprintf(stderr, "Treat end\n");

	//to json
	nlohmann::json json = res;
	std::cout << json << std::endl;

	return EXIT_SUCCESS;
}

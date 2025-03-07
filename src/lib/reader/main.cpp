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
#include "Profile.hpp"
#include "extractors/ExtractorHelpers.hpp"

/**********************************************************/
using namespace MALTReader;

/**********************************************************/
int main(int argc, char ** argv)
{
	Profile profile(argv[1], true);
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

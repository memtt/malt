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
#include "format/MaltProfile.hpp"
#include <nlohmann/json.hpp>
#include "loader/FileReader.hpp"

/**********************************************************/
using namespace MALTReader;

/**********************************************************/
int main(int argc, char ** argv)
{
	//open stream
	FileReader reader(argv[1], true);

	//load it
	printf("Parsing JSON...\n");
	nlohmann::json data = nlohmann::json::parse(begin(reader), end(reader));
	printf("Extracting infos...\n");
	MALTFormat::MaltProfile profile = data;
	printf("Done.\n");
}

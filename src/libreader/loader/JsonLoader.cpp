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
//keep debug for error handling for now
#undef NDEBUG
#include <cassert>
//to parse json
#include <fstream>
//local
#include "FileReader.hpp"
#include "JsonLoader.hpp"

/**********************************************************/
using namespace MALTFormat;
using json = nlohmann::json;

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
JsonLoader::JsonLoader(const std::string & fname, bool progressBar)
{
	this->loadFile(fname, progressBar);
}

/**********************************************************/
JsonLoader::~JsonLoader(void)
{

}

/**********************************************************/
void JsonLoader::loadFile(const std::string & fname, bool progressBar)
{
	//open stream
	FileReader reader(fname, progressBar);

	//load it
	printf("Parsing JSON...\n");
	json data = json::parse(begin(reader), end(reader));
	printf("Extracting infos...\n");
	this->profile = data;
	printf("Done.\n");
}

/**********************************************************/
MaltProfile & JsonLoader::getProfile(void)
{
	return this->profile;
}

/**********************************************************/
const MaltProfile & JsonLoader::getProfile(void) const
{
	return this->profile;
}

}

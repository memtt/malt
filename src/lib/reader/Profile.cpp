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
#include "loader/FileReader.hpp"
#include "Profile.hpp"

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
Profile::Profile(const std::string & fname, bool loadProgressBar)
{
	//open stream
	FileReader reader(fname, loadProgressBar);

	//load it
	nlohmann::json data = nlohmann::json::parse(begin(reader), end(reader));
	this->profile = data;

	//build extractor
	this->extractor = new Extractor(this->profile);
}

/**********************************************************/
Profile::~Profile(void)
{
	delete this->extractor;
	this->extractor = nullptr;
}

/**********************************************************/
const Extractor & Profile::getExtractor(void) const
{
	return *this->extractor;
}

}


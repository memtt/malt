/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/api/Profile.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
*    AUTHOR   : Sébastien Valat - 2026
***********************************************************/

/**********************************************************/
#include "loader/FileReader.hpp"
#include "Profile.hpp"

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
Profile::Profile(const std::string & fname, bool loadProgressBar)
	:fname(fname)
{
	//load it
	#ifdef USE_INTERNAL_JSON_IN
		MALTJson::JsonFile file(fname, loadProgressBar);
		file.getRoot().get_to(this->profile);
	#else
		//open stream
		FileReader reader(fname, loadProgressBar);
		nlohmann::json data = nlohmann::json::parse(begin(reader), end(reader));
		this->profile = data;
	#endif

	//load trace
	std::string traceFName = TraceReader::calcNameFromJson(fname);
	if (TraceReader::fileExist(traceFName)) {
		this->traceReader = new TraceReader(traceFName, loadProgressBar);
	}

	//build extractor
	this->extractor = new Extractor(this->profile, fname);
}

/**********************************************************/
Profile::~Profile(void)
{
	if (this->traceReader != nullptr)
		delete this->traceReader;
	delete this->extractor;
	this->extractor = nullptr;
}

/**********************************************************/
const Extractor & Profile::getExtractor(void) const
{
	return *this->extractor;
}

/**********************************************************/
const std::string & Profile::getFileName(void) const
{
	return this->fname;
}

}


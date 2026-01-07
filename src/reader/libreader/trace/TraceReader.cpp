/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 10/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/trace/TraceReader.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <cassert>
#include <cstring>
#include "TraceReader.hpp"

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
static const size_t CST_BUFFER_SIZE = 1024*1024;

/**********************************************************/
TraceReader::TraceReader(const std::string & fname, bool progress)
{
	this->load(fname, progress);
}

/**********************************************************/
TraceReader::~TraceReader(void)
{
}

/**********************************************************/
void TraceReader::load(const std::string & fname, bool progress)
{
	//open
	FILE * fp = fopen(fname.c_str(), "r");
	if (fp == nullptr) {
		perror(fname.c_str());
		abort();
	}

	//get file size
	size_t fsize = this->getFileSize(fp);

	//preallocate mem
	size_t estimatedEntries = fsize / sizeof(MALT::AllocTracerEvent);
	this->trace.reserve(estimatedEntries);

	//if progress
	ProgressBar * progressBar = nullptr;
	if (progress)
		progressBar = new ProgressBar(fsize, MALT_DRAW_WIDTH_AUTO, 1);

	//read
	size_t lastPosition = 0;
	while(!feof(fp)) {
		MALT::AllocTracerEvent event;
		if (event.fromFilePointer(fp)) {
			this->trace.push_back(event);
		} else {
			break;
		}
		size_t position = ftell(fp);
		if (progressBar != nullptr)
			progressBar->progress(position - lastPosition);
		lastPosition = position;
	}

	//progress
	if (progressBar != nullptr)
		delete progressBar;
}

/**********************************************************/
size_t TraceReader::getFileSize(FILE * fp)
{
	//get current pos
	ssize_t currentPos = ftell(fp);
	assert(currentPos >= 0);

	//seek to end
	const int status = fseek(fp, 0L, SEEK_END);
	assert(status == 0);

	//get pos
	ssize_t endPos = ftell(fp);
	assert(endPos >= 0);

	//seek back
	const int status2 = fseek(fp, currentPos, SEEK_SET);
	assert(status2 == 0);

	//ok
	return endPos;
}

/**********************************************************/
std::string TraceReader::calcNameFromJson(const std::string & fname)
{
	std::string copy = fname;
	copy.replace(copy.size() - 5, 5, ".trace");
	return copy;
}

/**********************************************************/
bool TraceReader::fileExist(const std::string & fname)
{
	FILE * fp = fopen(fname.c_str(), "r");
	if (fp == nullptr)
		return false;
	fclose(fp);
	return true;
}

}

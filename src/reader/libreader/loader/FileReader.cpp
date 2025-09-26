/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/loader/FileReader.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <cassert>
#include <cstring>
#include "FileReader.hpp"

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
static const size_t CST_BUFFER_SIZE = 1024*1024;

/**********************************************************/
FileReader::FileReader(const std::string & fname, bool progress)
{
	//open
	this->fp = fopen(fname.c_str(), "r");
	assert(this->fp != nullptr);

	//allocate buffer
	this->buffer = new char[CST_BUFFER_SIZE];

	//if progress
	if (progress) {
		size_t fsize = this->getFileSize(this->fp);
		this->progressBar = new ProgressBar(fsize, MALT_DRAW_WIDTH_AUTO, 1);
	}

	//load next
	this->advance();
}

/**********************************************************/
FileReader::~FileReader(void)
{
	//close & delete
	fclose(this->fp);
	delete [] this->buffer;
	if (this->progressBar != nullptr) 
		delete this->progressBar;

	//reset
	this->fp = nullptr;
	this->progressBar = nullptr;
}

/**********************************************************/
void FileReader::advance()
{
	//if eof
	if (this->isEof) {
		this->current = '\0';
		return;
	}

	//need load
	if (this->cursor == this->bufferFill) {
		ssize_t status = fread(this->buffer, 1, CST_BUFFER_SIZE, this->fp);
		this->current = '\0';
		this->cursor = 0;
		if (status == 0) {
			this->isEof = true;
			this->bufferFill = 0;
			memset(this->buffer, 0, CST_BUFFER_SIZE);
			return;
		} else {
			this->bufferFill = status;
		}
	}

	//in buffer
	assert(this->cursor < this->bufferFill);
	this->current = this->buffer[this->cursor++];

	//progress
	if (this->progressBar != nullptr)
		this->progressBar->progress(1);
}

/**********************************************************/
const char& FileReader::get_current()
{
	return this->current;
}

/**********************************************************/
size_t FileReader::getFileSize(FILE * fp)
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


}

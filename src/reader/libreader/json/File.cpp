/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 10/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/json/File.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <cassert>
#include <sys/mman.h>
#include <cstring>
#include "File.hpp"

/**********************************************************/
namespace MALTJson
{

/**********************************************************/
JsonFile::JsonFile(const std::string & fname, bool progress)
{
	//save
	this->filename = fname;
	this->progress = progress;

	//open
	FILE * fp = fopen(fname.c_str(), "r");
	if (fp == nullptr) {
		throw std::runtime_error(std::string("Fail to opening profile file :") + fname + std::string(" : ") + std::string(strerror(errno)));
	}

	//get file size
	ssize_t status = fseek(fp, 0, SEEK_END);
	if (status != 0)
		throw std::runtime_error(std::string("Fail to get file size : " + std::string(strerror(errno)) + std::string(" : ") + fname));
	ssize_t size = ftell(fp);
	if (size < 0 || size > 1024L*1024L*1024L*1024L)
		throw std::runtime_error(std::string("Fail to get file size : " + std::string(strerror(errno)) + std::string(" : ") + fname));

	//round to 4K limit
	const size_t pageSize = 4096;
	if (size % pageSize != 0)
		size += size + pageSize - size % pageSize;

	//map
	const char * data = (char*)mmap(nullptr, size, PROT_READ, MAP_FILE|MAP_PRIVATE, fileno(fp), 0);
	if (data == MAP_FAILED)
		throw std::runtime_error(std::string("Fail to mmap the file : " + std::string(strerror(errno)) + std::string(" : ") + fname));

	//assign
	this->data.value = data;
	this->data.len = size;

	//parse
	fprintf(stderr, "parsing...\n");
	this->parse();
	fprintf(stderr, "end parsing...\n");
}

/**********************************************************/
JsonFile::~JsonFile(void)
{
	ssize_t status = munmap((void*)this->data.value, this->data.len);
	assert(status == 0);
}

/**********************************************************/
void JsonFile::parse(void)
{
	//build cursor
	JsonFileCursor cursor;
	cursor.fname = &this->filename;
	cursor.data = &this->data;

	//progress bar
	if (this->progress)
		cursor.progressBar = new MALTReader::ProgressBar(strlen(cursor.data->value), MALT_DRAW_WIDTH_AUTO, 1);

	//get next
	JsonCharType next = cursor.forwardNextStart();
	switch (next) {
		case JSON_CHAR_ARRAY_START:
		case JSON_CHAR_OBJECT_START:
			cursor.parseChild(this->root);
			break;
		default:
			cursor.raiseError("Invalid start, should be object or array !");
			break;
	}

	//end
	if (this->progress) {
		delete cursor.progressBar;
	}
}

}

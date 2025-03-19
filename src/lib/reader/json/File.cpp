#include <cassert>
#include <sys/mman.h>
#include "File.hpp"

namespace MALTJson
{

JsonFile::JsonFile(const std::string & fname)
{
	//save
	this->filename = fname;

	//open
	FILE * fp = fopen(fname.c_str(), "r");
	assert(fp != nullptr);

	//get file size
	ssize_t status = fseek(fp, 0, SEEK_END);
	assert(status == 0);
	ssize_t size = ftell(fp);
	assert(size >= 0);

	//round to 4K limit
	const size_t pageSize = 4096;
	if (size % pageSize != 0)
		size += size + pageSize - size % pageSize;

	//map
	const char * data = (char*)mmap(nullptr, size, PROT_READ, MAP_FILE|MAP_PRIVATE, fileno(fp), 0);
	assert(data != MAP_FAILED);

	//assign
	this->data.value = data;
	this->data.len = size;

	//parse
	fprintf(stderr, "parsing...\n");
	this->parse();
	fprintf(stderr, "end parsing...\n");
}

JsonFile::~JsonFile(void)
{
	ssize_t status = munmap((void*)this->data.value, this->data.len);
	assert(status == 0);
}

void JsonFile::parse(void)
{
	//build cursor
	JsonFileCursor cursor;
	cursor.fname = &this->filename;
	cursor.data = &this->data;

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
}

}

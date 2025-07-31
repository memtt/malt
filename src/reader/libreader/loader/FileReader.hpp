/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/Backtrace.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_READER_FILE_READER_HPP
#define MALT_READER_FILE_READER_HPP

/**********************************************************/
#include <cstdio>
#include <string>
#include "ProgressBar.hpp"

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
class FileReader
{
	public:
		FileReader(const std::string & fname, bool progress = false);
		~FileReader(void);
		void advance();
		const char& get_current();
	private:
		static size_t getFileSize(FILE * fp);
	private:
		FILE * fp{nullptr};
		ProgressBar * progressBar{nullptr};
		size_t cursor{0};
		bool isEof{false};
		char * buffer{nullptr};
		char current{'\0'};
		size_t bufferFill{0};
};

/**********************************************************/
struct FileReaderIterator {
	using difference_type = std::ptrdiff_t;
	using value_type = char;
	using pointer = const char*;
	using reference = const char&;
	using iterator_category = std::input_iterator_tag;

	FileReaderIterator& operator++() {
		target->advance();
		return *this;
	}

	bool operator!=(const FileReaderIterator& rhs) const {
		return rhs.target != target;
	}

	reference operator*() const {
		return target->get_current();
	}

	FileReader* target{nullptr};
};

/**********************************************************/
inline FileReaderIterator begin(FileReader& tgt) {
	FileReaderIterator res;
	res.target = &tgt;
	return res;
}

/**********************************************************/
inline FileReaderIterator end(const FileReader&) {
	return {};
}

}

#endif //MALT_READER_JSON_LOADER_HPP

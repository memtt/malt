/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 10/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/trace/TraceReader.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_READER_TRACE_READER_HPP
#define MALT_READER_TRACE_READER_HPP

/**********************************************************/
#include <cstdio>
#include <string>
#include "../loader/ProgressBar.hpp"
#include "core/AllocTraceFormat.hpp"

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
typedef std::vector<MALT::AllocTracerEvent> Trace;

/**********************************************************/
class TraceReader
{
	public:
		TraceReader(const std::string & fname, bool progress = false);
		~TraceReader(void);
		static std::string calcNameFromJson(const std::string & fname);
		static bool fileExist(const std::string & fname);
	private:
		void load(const std::string & fname, bool progress = false);
		static size_t getFileSize(FILE * fp);
	private:
		Trace trace;
};

}

#endif //MALT_READER_TRACE_READER_HPP

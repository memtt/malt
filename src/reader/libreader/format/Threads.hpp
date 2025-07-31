/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 03/2025
*    LICENSE  : CeCILL-C
*    FILE     : ./src/reader/libreader/format/Threads.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_FORMAT_THREADS_HPP
#define MALT_FORMAT_THREADS_HPP

/**********************************************************/
#include <string>
#include <vector>
#include "Json.hpp"
#include "Types.hpp"

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
struct ThreadsFuncStats
{
	size_t count;
	size_t sum;
	CyclesDuration time;
};

/**********************************************************/
struct ThreadStackMem
{
	size_t size;
	std::vector<LangAddress> stack;
	std::vector<size_t> mem;
	size_t total;
	struct {
		std::vector<size_t> min;
		std::vector<size_t> max;
		std::vector<size_t> index;
		std::vector<CyclesTime> timestamp;
		std::vector<LangAddress> location;
		CyclesTime peakTimesteamp;//TODO fix the name
		size_t peakMemory;
		size_t peakIndex;
		bool linearIndex;
	} timeprofiler;
};

/**********************************************************/
struct ThreadsStats
{
	ThreadsFuncStats malloc;
	ThreadsFuncStats posix_memalign;
	ThreadsFuncStats aligned_alloc;
	ThreadsFuncStats memalign;
	ThreadsFuncStats valloc;
	ThreadsFuncStats pvalloc;
	ThreadsFuncStats free;
	ThreadsFuncStats calloc;
	ThreadsFuncStats realloc;
};

/**********************************************************/
struct Thread
{
	ThreadStackMem stackMem;
	size_t cntMemOps;
	ThreadsStats stats;
};

/**********************************************************/
typedef std::vector<Thread> Threads;

/**********************************************************/
void from_json(const JsonIn & json, ThreadsFuncStats & value);
void to_json(nlohmann::json & json, const ThreadsFuncStats & value);
void from_json(const JsonIn & json, Thread & value);
void to_json(nlohmann::json & json, const Thread & value);
void from_json(const JsonIn & json, ThreadStackMem & value);
void to_json(nlohmann::json & json, const ThreadStackMem & value);
void from_json(const JsonIn & json, ThreadsStats & value);
void to_json(nlohmann::json & json, const ThreadsStats & value);

}

#endif //MALT_FORMAT_THREADS_HPP

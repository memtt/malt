/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/Backtrace.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_FORMAT_THREADS_HPP
#define MALT_FORMAT_THREADS_HPP

/**********************************************************/
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
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
struct Thread
{
	struct {
		size_t size;
		std::vector<LangAddress> stack;
		std::vector<size_t> mem;
		size_t total;
		struct {
			std::vector<size_t> min;
			std::vector<size_t> max;
			std::vector<size_t> index;
			std::vector<CyclesTime> timestamp;
			CyclesTime peakTimesteamp;//TODO fix the name
			size_t peakMemory;
			size_t peakIndex;
			bool linearIndex;
		} timeprofiler;
	} stackMem;
	size_t cntMemOps;
	struct {
		ThreadsFuncStats malloc;
		ThreadsFuncStats posix_memalign;
		ThreadsFuncStats aligned_alloc;
		ThreadsFuncStats memalign;
		ThreadsFuncStats valloc;
		ThreadsFuncStats pvalloc;
		ThreadsFuncStats free;
		ThreadsFuncStats calloc;
		ThreadsFuncStats realloc;
	} stats;
};

/**********************************************************/
typedef std::vector<Thread> Threads;

/**********************************************************/
void from_json(const nlohmann::json & json, ThreadsFuncStats & value);
void to_json(nlohmann::json & json, const ThreadsFuncStats & value);
void from_json(const nlohmann::json & json, Thread & value);
void to_json(nlohmann::json & json, const Thread & value);

}

#endif //MALT_FORMAT_THREADS_HPP

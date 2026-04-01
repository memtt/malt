/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 10/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/trace/TraceReader.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_READER_PROCESS_MODEL_HPP
#define MALT_READER_PROCESS_MODEL_HPP

/**********************************************************/
#include <cstdlib>
#include <map>
#include <vector>
#include "core/AllocTraceFormat.hpp"

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
struct ProcessMemDomainCounter
{
	size_t aliveChunks{0};
	size_t usedVirtualMemory{0};
};

/**********************************************************/
enum ProcessMemDomain
{
	PROCESS_MEM_DOMAIN_MALLOC,
	PROCESS_MEM_DOMAIN_MMAP,
	PROCESS_MEM_DOMAIN_C,
	PROCESS_MEM_DOMAIN_PYTHON_OBJ,
	PROCESS_MEM_DOMAIN_PYTHON_MEM,
	PROCESS_MEM_DOMAIN_PYTHON_RAW,
	PROCESS_MEM_DOMAIN_PYTHON,
	PROCESS_MEM_DOMAIN_ALL
};

/**********************************************************/
class ProcessMemModel
{
	public:
		ProcessMemModel(void);
		~ProcessMemModel(void);
		void playNextEvent(MALT::AllocTracerEvent & event);
		ProcessMemDomainCounter getCounters(ProcessMemDomain domain);
	private:
		void incr(ProcessMemDomain domain, size_t count, size_t mem);
		void decr(ProcessMemDomain domain, size_t count, size_t mem);
	private:
		ProcessMemDomainCounter counters[PROCESS_MEM_DOMAIN_ALL + 1];
		std::map<ProcessMemDomain, std::vector<ProcessMemDomain> > inheritance;
};

}

#endif //MALT_READER_PROCESS_MODEL_HPP

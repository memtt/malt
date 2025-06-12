/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 03/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/core/Trigger.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_TRIGGER_HPP
#define MALT_TRIGGER_HPP

/**********************************************************/
#include <cstdlib>
#include <thread>
#include "common/Options.hpp"
#include "portability/OS.hpp"

/**********************************************************/
namespace MALT
{

class Trigger
{
	public:
		Trigger(const Options & options, bool canHostSpyingThread = false);
		~Trigger(void);
		bool onSysUpdate(const OSMemUsage & memUsage) const;
		bool onProcMemUpdate(const OSProcMemUsage & mem) const;
		bool onRequestUpdate(size_t reqMem) const;
		bool onAllocOp(size_t nbAlloc) const;
		inline bool onThreadStackUpdate(size_t threadStackSize) const;
	private:
		size_t calcLimit(const std::string & value, size_t ref, const std::string & paramName = "Unknown");
		void runSpyingThread(void);
		void pauseAllButMe(void);
	private:
		const Options & options;
		size_t sysMemLimit{0};
		size_t appRssLimit{0};
		size_t appVirtLimit{0};
		size_t appReqLimit{0};
		size_t totalMemory{0};
		size_t threadStackLimit{0};
		size_t allocCountLimit{0};
		std::thread spyingThread;
		volatile bool spyingThreadKeepRunning{false};
};

/**********************************************************/
bool Trigger::onThreadStackUpdate(size_t threadStackSize) const
{
	//compute
	if (this->threadStackLimit > 0 && threadStackSize > this->threadStackLimit) {
		fprintf(stderr, "MALT: One thread reach stack limit at %zu bytes (> on-thread-stack-using=%s) [%zu / %zu]\n",
				threadStackSize,
				this->options.dumpOnThreadStackUsing.c_str(),
				threadStackSize,
				this->totalMemory
			);
		return true;
	}

	//ok
	return false;
}

}

#endif //MALT_TRIGGER_HPP

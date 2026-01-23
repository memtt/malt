/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/core/Trigger.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
*    AUTHOR   : Sébastien Valat - 2026
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

/**********************************************************/
/**
 * @brief Class used to trigger dump on some events inside MALT.
**/
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
		/** Reference to the option object to configurer the app **/
		const Options & options;
		/** Limit to apply on system memory, in bytes. **/
		size_t sysMemLimit{0};
		/** Limit to apply on RSS memory, in bytes. **/
		size_t appRssLimit{0};
		/** Limit to apply on virtual memory, in bytes. **/
		size_t appVirtLimit{0};
		/** Limit to appky on requested memory, in bytes. **/
		size_t appReqLimit{0};
		/** Limit to apply on the total memory. **/
		size_t totalMemory{0};
		/** Limit to apply on thread stack memory. **/
		size_t threadStackLimit{0};
		/** Limit to apply on number of memory allocations. **/
		size_t allocCountLimit{0};
		/** Handler for the watchdog thread. **/
		std::thread spyingThread;
		/** Used to notify the spyingThread to stop. **/
		volatile bool spyingThreadKeepRunning{false};
};

/**********************************************************/
/**
 * On thread stack update, perform the check.
 * @param threadStackSize Size of the current thread stack (in bytes).
 * @return Return true, if need to dump, false to continue running.
**/
bool Trigger::onThreadStackUpdate(size_t threadStackSize) const
{
	//compute
	if (this->threadStackLimit > 0 && threadStackSize > this->threadStackLimit) {
		fprintf(stderr, "MALT: One thread reach stack limit at %zu bytes (> on-thread-stack-using=%s) [%zu / %zu]\n",
				threadStackSize,
				this->options.dump.onThreadStackUsing.c_str(),
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

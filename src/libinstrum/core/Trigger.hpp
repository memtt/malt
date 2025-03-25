/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/core/ValgrindOutput.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2024
***********************************************************/

#ifndef MALT_TRIGGER_HPP
#define MALT_TRIGGER_HPP

/**********************************************************/
#include <cstdlib>
#include "common/Options.hpp"
#include "portability/OS.hpp"

/**********************************************************/
namespace MALT
{

class Trigger
{
	public:
		Trigger(const Options & options);
		~Trigger(void);
		bool onSysUpdate(const OSMemUsage & memUsage) const;
		bool onProcMemUpdate(const OSProcMemUsage & mem) const;
		bool onRequestUpdate(size_t reqMem) const;
		inline bool onThreadStackUpdate(size_t threadStackSize) const;
	private:
		size_t calcLimit(const std::string & value, size_t ref, const std::string & paramName = "Unknown");
	private:
		const Options & options;
		size_t sysMemLimit{0};
		size_t appRssLimit{0};
		size_t appVirtLimit{0};
		size_t appReqLimit{0};
		size_t totalMemory{0};
		size_t threadStackLimit{0};
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

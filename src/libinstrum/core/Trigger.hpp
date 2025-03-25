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
		bool onSysUpdate(const OSMemUsage & memUsage);
	private:
		size_t calcLimit(const std::string & value, size_t ref, const std::string & paramName = "Unknown");
	private:
		const Options & options;
		size_t sysMemLimit{0};
};

}

#endif //MALT_TRIGGER_HPP

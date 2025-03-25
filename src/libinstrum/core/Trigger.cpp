/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/core/ValgrindOutput.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

/**********************************************************/
#include <cstdio>
#include "common/Debug.hpp"
#include "Trigger.hpp"

/**********************************************************/
using namespace std;

/**********************************************************/
namespace MALT
{

/**********************************************************/
Trigger::Trigger(const Options & options)
	:options(options)
{
	//get sys mem
	OSMemUsage sysMem = OS::getMemoryUsage();

	//fill
	this->sysMemLimit = this->calcLimit(options.dumpOnSysFullAt, sysMem.totalMemory, "dump:on-sys-full-at");
}

/**********************************************************/
Trigger::~Trigger(void)
{

}

/**********************************************************/
bool Trigger::onSysUpdate(const OSMemUsage & memUsage)
{
	//trivial
	if (this->sysMemLimit == 0)
		return false;

	//compute
	size_t sysFullAt = memUsage.totalMemory - memUsage.freeMemory - memUsage.cached;
	if (sysFullAt > this->sysMemLimit) {
		fprintf(stderr, "MALT: System full at %zu bytes (> sys-full-at=%s) [%zu / %zu]\n",
				sysFullAt,
				this->options.dumpOnSysFullAt.c_str(),
				sysFullAt,
				memUsage.totalMemory
			);
		return true;
	}

	//ok
	return false;
}

/**********************************************************/
size_t Trigger::calcLimit(const std::string & value, size_t ref, const std::string & paramName)
{
	//trivial
	if (value.empty())
		return 0;

	//vars
	float valueFloat = 0.0;
	size_t valueSizet = 0;
	char last = value.back();

	//parse
	if (last == 'G' && sscanf(value.c_str(), "%fG", &valueFloat) == 1) {
			return valueFloat * 1024.0 * 1024*.0 * 1024.0;
	} else if (last == 'M' && sscanf(value.c_str(), "%fM", &valueFloat) == 1) {
			return valueFloat * 1024.0 * 1024.0;
	} else if (last == 'K' && sscanf(value.c_str(), "%fK", &valueFloat) == 1) {
			return valueFloat * 1024.0;
	} else if (last == '%' && sscanf(value.c_str(), "%f%%", &valueFloat) == 1) {
		return valueFloat * (float)ref / 100.0;
	} else if (last >= '0' && last <= '9' && sscanf(value.c_str(), "%zu", &valueSizet) == 1) {
			return valueSizet;
	} else {
		MALT_FATAL_ARG("Invalid format for parameter : %1 = %2").arg(paramName).arg(value).end();
		return 0;
	}
}

}

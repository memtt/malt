/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.0
*    DATE     : 03/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/core/Trigger.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <cstdio>
#include <thread>
#include <signal.h>
#include <sys/types.h>
#include <dirent.h>
#include "common/Debug.hpp"
#include "Trigger.hpp"
#include "state/GlobalState.hpp"
#include "state/LazyEnv.hpp"
#include "state/ReentranceGuard.hpp"

/**********************************************************/
using namespace std;

/**********************************************************/
namespace MALT
{

/**********************************************************/
Trigger::Trigger(const Options & options, bool canHostSpyingThread)
	:options(options)
{
	//get sys mem
	OSMemUsage sysMem = OS::getMemoryUsage();

	//fill
	this->totalMemory = sysMem.totalMemory;
	this->sysMemLimit = this->calcLimit(options.dumpOnSysFullAt, sysMem.totalMemory, "dump:on-sys-full-at");
	this->appRssLimit = this->calcLimit(options.dumpOnAppUsingRss, sysMem.totalMemory, "dump:on-app-using-rss");
	this->appVirtLimit = this->calcLimit(options.dumpOnAppUsingVirt, sysMem.totalMemory, "dump:on-app-using-virt");
	this->appReqLimit = this->calcLimit(options.dumpOnAppUsingReq, sysMem.totalMemory, "dump:on-app-using-req");
	this->threadStackLimit = this->calcLimit(options.dumpOnThreadStackUsing, sysMem.totalMemory, "dump:on-thread-stack-using");
	this->allocCountLimit = this->calcLimit(options.dumpOnAllocCount, 0, "dump:on-alloc-count");

	//start spying thread
	if (options.dumpWatchDog && canHostSpyingThread)
		this->runSpyingThread();
}

/**********************************************************/
Trigger::~Trigger(void)
{
	if (this->spyingThreadKeepRunning) {
		this->spyingThreadKeepRunning = false;
		this->spyingThread.join();
	}
}

/**********************************************************/
bool Trigger::onSysUpdate(const OSMemUsage & memUsage) const
{
	//compute
	size_t sysFullAt = memUsage.totalMemory - memUsage.freeMemory - memUsage.cached;
	if (this->sysMemLimit > 0 && sysFullAt > this->sysMemLimit) {
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
bool Trigger::onProcMemUpdate(const OSProcMemUsage & mem) const
{
	//check
	if (this->appRssLimit > 0 && mem.physicalMemory > this->appRssLimit) {
		fprintf(stderr, "MALT: RSS overpass limit : %zu bytes (> on-app-using-rss=%s) [%zu / %zu]\n",
				mem.physicalMemory,
				this->options.dumpOnAppUsingRss.c_str(),
				mem.physicalMemory,
				totalMemory
			);
		return true;
	}

	//check
	if (this->appVirtLimit > 0 && mem.virtualMemory > this->appVirtLimit) {
		fprintf(stderr, "MALT: Virtual Memory overpass limit : %zu bytes (> on-app-using-virt=%s) [%zu / %zu]\n",
				mem.virtualMemory,
				this->options.dumpOnAppUsingVirt.c_str(),
				mem.virtualMemory,
				totalMemory
			);
		return true;
	}

	//ok
	return false;
}

/**********************************************************/
bool Trigger::onAllocOp(size_t nbAlloc) const
{
	//check
	if (this->allocCountLimit > 0 && nbAlloc > this->allocCountLimit) {
		fprintf(stderr, "MALT: Number of Allocations requets overpass limit : %zu bytes (> on-alloc-count=%s)\n",
				nbAlloc,
				this->options.dumpOnAllocCount.c_str()
			);
		return true;
	}

	//ok
	return false;
}

/**********************************************************/
bool Trigger::onRequestUpdate(size_t reqMem) const
{
	//check
	if (this->appReqLimit > 0 && reqMem > this->appReqLimit) {
		fprintf(stderr, "MALT: Requested Memory overpass limit : %zu bytes (> on-app-using-req=%s) [%zu / %zu]\n",
				reqMem,
				this->options.dumpOnAppUsingReq.c_str(),
				reqMem,
				totalMemory
			);
		return true;
	}

	//ok
	return false;
}

/**********************************************************/
void maltDumpOnEvent();

/**********************************************************/
void Trigger::runSpyingThread(void)
{
	
	this->spyingThreadKeepRunning = true;
	this->spyingThread = std::thread([this](){
		fprintf(stderr, "MALT: Start active watch dog...\n");
		//get local TLS and check init
		LazyEnv env;
		ReentranceGuard guard(env);
		while(this->spyingThreadKeepRunning) {
			//get sys mem
			OSMemUsage sysMem = OS::getMemoryUsage();
			OSProcMemUsage procMem = OS::getProcMemoryUsage();
			if (gblState.profiler != nullptr)
				gblState.profiler->onUpdateMem(procMem, sysMem);
			if (this->onSysUpdate(sysMem)) {
				//pauseAllButMe();
				fprintf(stderr, "MALT: Watch dog triggering dump...\n");
				maltDumpOnEvent();
				return;
			}
			if (this->onProcMemUpdate(procMem)) {
				//pauseAllButMe();
				fprintf(stderr, "MALT: Watch dog triggering dump...\n");
				maltDumpOnEvent();
				return;
			}
		}
	});
}

/**********************************************************/
void Trigger::pauseAllButMe(void)
{
	//open dir
	fprintf(stderr, "MALT: Pause all threads except watch dog...\n");
	const char directory[] = "/proc/self/task";
	DIR* pdir = opendir (directory); 
	dirent *pent = NULL;
	pid_t cur = getpid();

	while (pent = readdir (pdir)) 
	{
		if (pent->d_name[0] == '.')
			continue;
		pid_t pid = atol(pent->d_name);
		fprintf(stderr, "MALT: Pause PID=%d\n", pid);
		if (pid != cur)
			kill(pid, SIGSTOP);
	}
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
			return (valueFloat * 1024.0 * 1024.0 * 1024.0);
	} else if (last == 'M' && sscanf(value.c_str(), "%fM", &valueFloat) == 1) {
			return valueFloat * 1024.0 * 1024.0;
	} else if (last == 'K' && sscanf(value.c_str(), "%fK", &valueFloat) == 1) {
			return valueFloat * 1024.0;
	} else if (last == '%' && ref > 0 && sscanf(value.c_str(), "%f%%", &valueFloat) == 1) {
		return valueFloat * (float)ref / 100.0;
	} else if (last >= '0' && last <= '9' && sscanf(value.c_str(), "%zu", &valueSizet) == 1) {
			return valueSizet;
	} else {
		MALT_FATAL_ARG("Invalid format for parameter : %1 = %2").arg(paramName).arg(value).end();
		return 0;
	}
}

}

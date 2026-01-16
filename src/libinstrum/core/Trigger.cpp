/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/core/Trigger.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
*    AUTHOR   : Sébastien Valat - 2026
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
/**
 * Constructor of the trigger class.
 * @param options Reference to the option object used to configure the app.
 * @param canHostSpyingThread allow spawning the spying thread or not depending
 * on the configuration.
**/
Trigger::Trigger(const Options & options, bool canHostSpyingThread)
	:options(options)
{
	//get sys mem
	OSMemUsage sysMem = OS::getMemoryUsage();

	//fill
	this->totalMemory = sysMem.totalMemory;
	this->sysMemLimit = this->calcLimit(options.dump.onSysFullAt, sysMem.totalMemory, "dump:on-sys-full-at");
	this->appRssLimit = this->calcLimit(options.dump.onAppUsingRss, sysMem.totalMemory, "dump:on-app-using-rss");
	this->appVirtLimit = this->calcLimit(options.dump.onAppUsingVirt, sysMem.totalMemory, "dump:on-app-using-virt");
	this->appReqLimit = this->calcLimit(options.dump.onAppUsingReq, sysMem.totalMemory, "dump:on-app-using-req");
	this->threadStackLimit = this->calcLimit(options.dump.onThreadStackUsing, sysMem.totalMemory, "dump:on-thread-stack-using");
	this->allocCountLimit = this->calcLimit(options.dump.onAllocCount, 0, "dump:on-alloc-count");

	//start spying thread
	if (options.dump.watchDog && canHostSpyingThread)
		this->runSpyingThread();
}

/**********************************************************/
/**
 * Destructor of the trigger classe, mostly stop the spying thread if
 * is running.
**/
Trigger::~Trigger(void)
{
	if (this->spyingThreadKeepRunning) {
		this->spyingThreadKeepRunning = false;
		this->spyingThread.join();
	}
}

/**********************************************************/
/**
 * Notify the trigger class that OS memory metrics has been updated, so we
 * need to check if we need to dump.
 * @param memUsage Define the memory metric for the current status.
 * @return True, if need to dump, false otherwise.
**/
bool Trigger::onSysUpdate(const OSMemUsage & memUsage) const
{
	//compute
	size_t sysFullAt = memUsage.totalMemory - memUsage.freeMemory - memUsage.cached;
	if (this->sysMemLimit > 0 && sysFullAt > this->sysMemLimit) {
		fprintf(stderr, "MALT: System full at %zu bytes (> sys-full-at=%s) [%zu / %zu]\n",
				sysFullAt,
				this->options.dump.onSysFullAt.c_str(),
				sysFullAt,
				memUsage.totalMemory
			);
		return true;
	}

	//ok
	return false;
}

/**********************************************************/
/**
 * Notify the trigger object when the proc mem has be readed again.
 * @param mem Define the memory metric.
 * @return True, if need to dump, false otherwise.
**/
bool Trigger::onProcMemUpdate(const OSProcMemUsage & mem) const
{
	//check
	if (this->appRssLimit > 0 && mem.physicalMemory > this->appRssLimit) {
		fprintf(stderr, "MALT: RSS overpass limit : %zu bytes (> on-app-using-rss=%s) [%zu / %zu]\n",
			mem.physicalMemory,
			this->options.dump.onAppUsingRss.c_str(),
			mem.physicalMemory,
			totalMemory
		);
		return true;
	}

	//check
	if (this->appVirtLimit > 0 && mem.virtualMemory > this->appVirtLimit) {
		fprintf(stderr, "MALT: Virtual Memory overpass limit : %zu bytes (> on-app-using-virt=%s) [%zu / %zu]\n",
				mem.virtualMemory,
				this->options.dump.onAppUsingVirt.c_str(),
				mem.virtualMemory,
				totalMemory
			);
		return true;
	}

	//ok
	return false;
}

/**********************************************************/
/**
 * Notify the trigger when a memory allocation has been treated.
 * @param nbAlloc Define the number of allocations performed in total.
 * @return True if needs to dump, false otherwise.
**/
bool Trigger::onAllocOp(size_t nbAlloc) const
{
	//check
	if (this->allocCountLimit > 0 && nbAlloc > this->allocCountLimit) {
		fprintf(stderr, "MALT: Number of Allocations requets overpass limit : %zu bytes (> on-alloc-count=%s)\n",
				nbAlloc,
				this->options.dump.onAllocCount.c_str()
			);
		return true;
	}

	//ok
	return false;
}

/**********************************************************/
/**
 * Notify the trigger class when the requested memory has changed.
 * @param reqMem to the current requested memory (total) in bytes.
 * @return True if needs to dump, false otherwise.
**/
bool Trigger::onRequestUpdate(size_t reqMem) const
{
	//check
	if (this->appReqLimit > 0 && reqMem > this->appReqLimit) {
		fprintf(stderr, "MALT: Requested Memory overpass limit : %zu bytes (> on-app-using-req=%s) [%zu / %zu]\n",
				reqMem,
				this->options.dump.onAppUsingReq.c_str(),
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
/**
 * Starts the spying thread.
**/
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
/**
 * Try to pause all the other threads expecpt self.
**/
void Trigger::pauseAllButMe(void)
{
	//open dir
	fprintf(stderr, "MALT: Pause all threads except watch dog...\n");
	const char directory[] = "/proc/self/task";
	DIR* pdir = opendir (directory); 
	dirent *pent = NULL;
	pid_t cur = getpid();

	while ((pent = readdir (pdir))) 
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
/**
 * Calculare the limit from a given parameter.
 * It allows the semantif :
 *   - 100G
 *   - 100M
 *   - 100K
 *   - 50%
 * @param value The string value defining the paramater value.
 * @param ref The refrence max memory (for %).
 * @param paramName Name of the parameter to print in debug if the value has wrong format.
 * @return The size in bytes.
**/
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

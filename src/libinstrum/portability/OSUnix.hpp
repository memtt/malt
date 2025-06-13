/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.0
*    DATE     : 06/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/portability/OSUnix.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2015 - 2022
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_OS_UNIX_HPP
#define MALT_OS_UNIX_HPP

/**********************************************************/
//standard
#include <string>
#include <cstdlib>
#include <iostream>

/**********************************************************/
namespace MALT
{

/**********************************************************/
struct LinuxInternalStatm
{
	size_t size;
	size_t resident;
	size_t share;
	size_t text;
	size_t lib;
	size_t data;
	size_t dirty;
};

/**********************************************************/
struct OSProcMemUsage
{
	size_t virtualMemory;
	size_t physicalMemory;
};

/**********************************************************/
struct OSMemUsage
{
	size_t totalMemory;
	size_t freeMemory;
	size_t buffers;
	size_t cached;
	size_t swap;
	size_t totalSwap;
	size_t directMap4K;
	size_t directMap2M;
};

/**********************************************************/
class OSUnix
{
	public:
		static OSProcMemUsage getProcMemoryUsage(void);
		static OSMemUsage getMemoryUsage(void);
		static unsigned int getPID(void);
		static std::string getExeName(void);
		static std::string getHostname(void);
		static std::string getDateTime(void);
		static std::string getCmdLine(void);
		static std::string getSignalName(int sig);
		static void printAvailSigs(std::ostream & out = std::cout);
		static int getSignalFromName(const std::string & signame);
		static void setSigHandler(void (*handler)(int s), const std::string & signame);
		static void setSigHandler(void (*handler)(int s), int sigid);
		static std::string loadTextFile(const std::string & file);
		static void * mmap(size_t size, bool populate = false);
		static void munmap(void * ptr,size_t size);
		static size_t getASLROffset(void * instrAddr);
		static bool hasASLREnabled(void);
		static size_t getFileSize(const std::string & fname);
};

}

#endif //MALT_OS_UNIX_HPP

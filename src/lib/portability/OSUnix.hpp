/*****************************************************
             PROJECT  : MALT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_OS_UNIX_HPP
#define MALT_OS_UNIX_HPP

/********************  HEADERS  *********************/
//standard
#include <string>
#include <cstdlib>

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*********************  STRUCT  *********************/
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

/********************  STRUCT  **********************/
struct OSProcMemUsage
{
	size_t virtualMemory;
	size_t physicalMemory;
};

/********************  STRUCT  **********************/
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

/*********************  CLASS  **********************/
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
		static void setSigKillHandler(void (*handler)(int s));
		static std::string loadTextFile(const std::string & file);
};

}

#endif //MALT_OS_UNIX_HPP

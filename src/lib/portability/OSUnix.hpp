/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_OS_UNIX_HPP
#define MATT_OS_UNIX_HPP

/********************  HEADERS  *********************/
//standard
#include <string>
#include <cstdlib>

/*******************  NAMESPACE  ********************/
namespace MATT
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
struct OSMemUsage
{
	size_t virtualMemory;
	size_t physicalMemory;
};

/*********************  CLASS  **********************/
class OSUnix
{
	public:
		static OSMemUsage getMemoryUsage(void);
		static unsigned int getPID(void);
		static std::string getExeName(void);
};

}

#endif //MATT_OS_UNIX_HPP

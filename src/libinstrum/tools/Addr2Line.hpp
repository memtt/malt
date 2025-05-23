/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/Backtrace.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

#ifndef MALT_ADDR2LINE_HPP
#define MALT_ADDR2LINE_HPP

/**********************************************************/
#include <string>
#include <list>
//internal
#include <stacks/LangAddress.hpp>
#include <common/StringIdDictionnary.hpp>
#include <portability/LinuxProcMapReader.hpp>

/**********************************************************/
namespace MALT
{

/**********************************************************/
/**
 * Define a call site.
 * with its source informations extracted from debug symbols.
**/
struct CallSite
{
	CallSite(LinuxProcMapEntry * mapEntry = NULL);
	/** Define the line of the call site. **/
	int line;
	/** Define the file ID (pointing the string) of the call site. **/
	int file;
	/** Define the function ID (pointing the string) of the call site. **/
	int function;
	/** Define the binary ID (pointing the string) of the call site. */
	int binary;
	/** Pointer to the proc map entry related to the call site. **/
	LinuxProcMapEntry * mapEntry;
};

/**********************************************************/
struct Addr2LineTask
{
	LangAddress address;
	CallSite * callSite;
};

/**********************************************************/
typedef std::vector<Addr2LineTask> Addr2LineTaskVector;

/**********************************************************/
class Addr2Line
{
	public:
		Addr2Line(StringIdDictionnary & dict, const std::string & elfFile, size_t aslrOffset, size_t bucketSize = 100);
		bool addTask(const LangAddress & address, CallSite * callSite);
		bool run(void);
		bool isFull(void) const;
	private:
		bool loadEntry(CallSite & callSite, FILE * fp);
		std::string buildCommandLine(void) const;
	private:
		StringIdDictionnary & dict;
		std::string elfFile;
		int elfFileId{-1};
		size_t bucketSize;
		size_t aslrOffset;
		Addr2LineTaskVector tasks;
		char bufferFunc[512*4096];
		char bufferFile[512*4096];
};

}

#endif //MALT_ADDR2LINE_HPP

/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//standards
#include <cstdio>
//unix specific files
#include <sys/types.h>
#include <unistd.h>
#include <libgen.h>
//internals
#include <common/Debug.hpp>
#include "Mutex.hpp"
#include "OSUnix.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  CONSTS  *********************/
static const char * cstStatmFile = "/proc/self/statm";
static const char * cstExeFile = "/proc/self/exe";

/*******************  FUNCTION  *********************/
OSMemUsage OSUnix::getMemoryUsage(void)
{
	//use some static 
	static FILE * fp = NULL;
	static StaticMutex gblStatmMutex = MATT_STATIC_MUTEX_INIT;
	OSMemUsage finalRes = {0,0};
	
	//secure for threads
	MATT_START_CRITICAL(gblStatmMutex)
		//try to open if not already
		if (fp == NULL)
		{
			fp = fopen(cstStatmFile,"r");
			assumeArg(fp != NULL,"Fail to open %1 : %2").arg(cstStatmFile).argStrErrno().end();
		}
		
		//seek to start
		int res = fseek(fp,0,SEEK_SET);
		assumeArg(fp != NULL,"Fail to seek into %1 : %2").arg(cstStatmFile).argStrErrno().end();
		
		//read
		LinuxInternalStatm stat;
		res = fscanf(fp,"%lu %lu %lu %lu %lu %lu %lu\n",
               &stat.size,&stat.resident, &stat.share, &stat.text, &stat.lib, &stat.data, &stat.dirty);
        assumeArg(res == 7,"Fail to read %1 entries, get only %2 entries where %3 is expected !").arg(cstStatmFile).arg(res).arg(7);
		
		//extract values
		finalRes.virtualMemory  = stat.size * 4096UL;
		finalRes.physicalMemory = stat.resident * 4096UL;
	MATT_END_CRITICAL
	
	//return
	return finalRes;
}

/*******************  FUNCTION  *********************/
unsigned int OSUnix::getPID(void)
{
	return getpid();
}

/*******************  FUNCTION  *********************/
std::string OSUnix::getExeName(void)
{
	//buffer to read link
	char buffer[2048];
	
	//read
	size_t res = readlink(cstExeFile,buffer,sizeof(buffer));
	assumeArg(res != -1,"Fail to read link %1 : %2 !").arg(cstExeFile).argStrErrno().end();
	assume(res < sizeof(buffer),"Fail to read link to get exe name. Maybe buffer is too small.");
	
	//put \0
	buffer[res] = '\0';

	//extract exe from path
	char * name = basename(buffer);
	return name;
}

}

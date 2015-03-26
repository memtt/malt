/*****************************************************
             PROJECT  : MALT
             VERSION  : 0.1.0
             DATE     : 03/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//standards
#include <cstdio>
#include <cstdlib>
//unix specific files
#include <sys/types.h>
#include <unistd.h>
#include <libgen.h>
#include <signal.h>
//internals
#include <common/Debug.hpp>
#include "Mutex.hpp"
#include "OSUnix.hpp"

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*********************  CONSTS  *********************/
static const char * cstMeminfoFile = "/proc/meminfo";
static const char * cstStatmFile = "/proc/self/statm";
static const char * cstExeFile = "/proc/self/exe";

/*******************  FUNCTION  *********************/
OSMemUsage OSUnix::getMemoryUsage(void)
{
	//use some static to avoid many open/close
	static FILE * fp = NULL;
	static StaticMutex gblMeminfoMutex = MALT_STATIC_MUTEX_INIT;
	OSMemUsage finalRes = {0,0,0,0,0,0,0};
	char buffer[512];
	
	//secure for threads
	MALT_START_CRITICAL(gblMeminfoMutex)
		//try to open if not already
		if (fp == NULL)
		{
			fp = fopen(cstMeminfoFile,"r");
			assumeArg(fp != NULL,"Fail to open %1 : %2").arg(cstMeminfoFile).argStrErrno().end();
		}
		
		//seek to start
		int res = fseek(fp,0,SEEK_SET);
		assumeArg(res == 0,"Fail to seek into %1 : %2").arg(cstMeminfoFile).argStrErrno().end();
		
		//read
		while (!feof(fp))
		{
			//read line into buffer
			char * res = fgets(buffer,sizeof(buffer),fp);
			
			//get end
			if (res == NULL)
			{
				MALT_ASSERT(feof(fp));
				break;
			}
			
			//extract
			size_t value;
			if (sscanf(buffer,"MemTotal: %lu kB",&value) == 1)
			{
				finalRes.totalMemory = value * 1024;
			} else if (sscanf(buffer,"MemFree: %lu kB",&value) == 1) {
				finalRes.freeMemory = value * 1024;
			} else if (sscanf(buffer,"buffers: %lu kB",&value) == 1) {
				finalRes.buffers = value * 1024;
			} else if (sscanf(buffer,"Cached: %lu kB",&value) == 1) {
				finalRes.cached = value * 1024;
			} else if (sscanf(buffer,"SwapTotal: %lu kB",&value) == 1) {
				finalRes.totalSwap = value * 1024;
			} else if (sscanf(buffer,"SwapFree: %lu kB",&value) == 1) {
				finalRes.swap = value * 1024;
			} else if (sscanf(buffer,"DirectMap4k: %lu kB",&value) == 1) {
				finalRes.directMap4K = value * 1024;
			} else if (sscanf(buffer,"DirectMap2M: %lu kB",&value) == 1) {
				finalRes.directMap2M = value * 1024;
			}
		}
		
		//compute swap
		finalRes.swap = finalRes.totalSwap - finalRes.swap;
	MALT_END_CRITICAL
	
	//return
	return finalRes;
}

/*******************  FUNCTION  *********************/
OSProcMemUsage OSUnix::getProcMemoryUsage(void)
{
	//use some static 
	static FILE * fp = NULL;
	static StaticMutex gblStatmMutex = MALT_STATIC_MUTEX_INIT;
	OSProcMemUsage finalRes = {0,0};
	
	//secure for threads
	MALT_START_CRITICAL(gblStatmMutex)
		//try to open if not already
		if (fp == NULL)
		{
			fp = fopen(cstStatmFile,"r");
			assumeArg(fp != NULL,"Fail to open %1 : %2").arg(cstStatmFile).argStrErrno().end();
		}
		
		//seek to start
		int res = fseek(fp,0,SEEK_SET);
		assumeArg(res == 0,"Fail to seek into %1 : %2").arg(cstStatmFile).argStrErrno().end();
		
		//read
		LinuxInternalStatm stat;
		res = fscanf(fp,"%lu %lu %lu %lu %lu %lu %lu\n",
               &stat.size,&stat.resident, &stat.share, &stat.text, &stat.lib, &stat.data, &stat.dirty);
        assumeArg(res == 7,"Fail to read %1 entries, get only %2 entries where %3 is expected !").arg(cstStatmFile).arg(res).arg(7);
		
		//extract values
		finalRes.virtualMemory  = stat.size * 4096UL;
		finalRes.physicalMemory = stat.resident * 4096UL;
	MALT_END_CRITICAL
	
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
	assumeArg(res != (size_t)-1,"Fail to read link %1 : %2 !").arg(cstExeFile).argStrErrno().end();
	assume(res < sizeof(buffer),"Fail to read link to get exe name. Maybe buffer is too small.");
	
	//put \0
	buffer[res] = '\0';

	//extract exe from path
	char * name = basename(buffer);
	return name;
}

/*******************  FUNCTION  *********************/
void OSUnix::setSigKillHandler(void (*handler)(int) )
{
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);
}

/*******************  FUNCTION  *********************/
std::string OSUnix::getHostname(void)
{
	char buffer[4096];
	int res = gethostname(buffer,sizeof(buffer));
	assume(res == 0,"Failed to read hostname with getHostname() !");
	return buffer;
}

/*******************  FUNCTION  *********************/
std::string OSUnix::getDateTime(void)
{
	//vars
	char buffer[200];
	time_t t;
	struct tm *tmp;

	//get time
	t = time(NULL);
	tmp = localtime(&t);
	assume(tmp != NULL,"Failed to get time with localtime() !");
	
	//convert to string format
	int res = strftime(buffer, sizeof(buffer), "%F %R", tmp);
	assume(res > 0,"Failed to convert time to string format with strftime() !");
	
    return buffer;
}

/*******************  FUNCTION  *********************/
std::string OSUnix::getCmdLine(void)
{
	return loadTextFile("/proc/self/cmdline");
}

/*******************  FUNCTION  *********************/
std::string OSUnix::loadTextFile(const std::string& file)
{
	char buffer[1025];
	std::string res;
	
	//open an check
	FILE * fp = fopen(file.c_str(),"r");
	assumeArg(fp != NULL,"Failed to read file %1 : %2 !")
		.arg(file)
		.argStrErrno()
		.end();

	//load content
	while (!feof(fp)){
		size_t tmp = fread(buffer,1,sizeof(buffer),fp);
		if (tmp > 0)
		{
			buffer[tmp] = '\0';
			res += buffer;
		}
	}
	
	//close file
	fclose(fp);
	
	//finish
	return res;
}

}

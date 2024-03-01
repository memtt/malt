/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//standards
#include <cstdio>
#include <cstdlib>
//unix specific files
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <libgen.h>
#include <signal.h>
#include <string.h>
//internals
#include <common/Debug.hpp>
#include "Mutex.hpp"
#include "OSUnix.hpp"

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*********************  MACROS  *********************/
#define MALT_DEF_SIGNAL(x) { #x, x }

/********************  STRUCT  **********************/
struct SigNamesEntry
{
	const char * name;
	int sig;
};

/*********************  CONSTS  *********************/
static const char * cstMeminfoFile = "/proc/meminfo";
static const char * cstStatmFile = "/proc/self/statm";
static const char * cstExeFile = "/proc/self/exe";
static const int cstSigCnt = 16;
static SigNamesEntry cstSigNames[cstSigCnt] = {
	MALT_DEF_SIGNAL(SIGUSR1),
	MALT_DEF_SIGNAL(SIGUSR2),
	MALT_DEF_SIGNAL(SIGINT),
	MALT_DEF_SIGNAL(SIGQUIT),
	MALT_DEF_SIGNAL(SIGILL),
	MALT_DEF_SIGNAL(SIGABRT),
	MALT_DEF_SIGNAL(SIGKILL),
	MALT_DEF_SIGNAL(SIGSEGV),
	MALT_DEF_SIGNAL(SIGPIPE),
	MALT_DEF_SIGNAL(SIGALRM),
	MALT_DEF_SIGNAL(SIGTERM),
	MALT_DEF_SIGNAL(SIGCHLD),
	MALT_DEF_SIGNAL(SIGCONT),
	MALT_DEF_SIGNAL(SIGSTOP),
	MALT_DEF_SIGNAL(SIGTSTP),
	MALT_DEF_SIGNAL(SIGFPE),
};

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
			if (sscanf(buffer,"MemTotal: %zu kB",&value) == 1)
			{
				finalRes.totalMemory = value * 1024;
			} else if (sscanf(buffer,"MemFree: %zu kB",&value) == 1) {
				finalRes.freeMemory = value * 1024;
			} else if (sscanf(buffer,"buffers: %zu kB",&value) == 1) {
				finalRes.buffers = value * 1024;
			} else if (sscanf(buffer,"Cached: %zu kB",&value) == 1) {
				finalRes.cached = value * 1024;
			} else if (sscanf(buffer,"SwapTotal: %zu kB",&value) == 1) {
				finalRes.totalSwap = value * 1024;
			} else if (sscanf(buffer,"SwapFree: %zu kB",&value) == 1) {
				finalRes.swap = value * 1024;
			} else if (sscanf(buffer,"DirectMap4k: %zu kB",&value) == 1) {
				finalRes.directMap4K = value * 1024;
			} else if (sscanf(buffer,"DirectMap2M: %zu kB",&value) == 1) {
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
		res = fscanf(fp,"%zu %zu %zu %zu %zu %zu %zu\n", &stat.size,&stat.resident, &stat.share, &stat.text, &stat.lib, &stat.data, &stat.dirty);
		if (res != 7)
		{
			finalRes.virtualMemory  = 0;
			finalRes.physicalMemory = 0;
		}
		//assumeArg(res == 7,"Fail to read %1 entries, get only %2 entries where %3 is expected !").arg(cstStatmFile).arg(res).arg(7).end();i
		
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
std::string OSUnix::getSignalName(int sig)
{
	//search to convert
	for (int i = 0 ; i < cstSigCnt ; i++) {
		if (cstSigNames[i].sig == sig) {
			return cstSigNames[i].name;
		}
	}

	//not found
	return "UNKNOWN";
}

/*******************  FUNCTION  *********************/
int OSUnix::getSignalFromName(const std::string & signame)
{
	//search to convert
	for (int i = 0 ; i < cstSigCnt ; i++) {
		if (signame == cstSigNames[i].name) {
			return cstSigNames[i].sig;
		}
	}

	//fail
	MALT_FATAL_ARG("Invalid signal to attach handler for dummping profile: %1").arg(signame).end();
	return -1;
}

/*******************  FUNCTION  *********************/
void OSUnix::printAvailSigs(std::ostream & out)
{
	for (int i = 0 ; i < cstSigCnt ; i++) {
		out << "MALT: supported signal: " << cstSigNames[i].name << " (" << cstSigNames[i].sig << ")" << std::endl;
	}
}

/*******************  FUNCTION  *********************/
void OSUnix::setSigHandler(void (*handler)(int), const std::string & signame )
{
	//convert and call
	int sig = getSignalFromName(signame);
	setSigHandler(handler, sig);
}

/*******************  FUNCTION  *********************/
void OSUnix::setSigHandler(void (*handler)(int), int sigid )
{
	//setup
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);

	//setup
	int status = sigaction(sigid, &sigIntHandler, NULL);
	assumeArg(status == 0, "Fail to install signal %1: %2").arg(getSignalName(sigid)).argStrErrno().end();
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

/*******************  FUNCTION  *********************/
void* OSUnix::mmap(size_t size, bool populate)
{
	void * res;
	if (populate) {
		//MAP_POPULATE avail under Linux >= 2.6.23
		//not avail on other unix & macosx
		#ifdef MAP_POPULATE
			res = ::mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_ANON|MAP_PRIVATE|MAP_POPULATE,0,0);
		#else
			res = ::mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_ANON|MAP_PRIVATE,0,0);
			// touch one byte per page to force mapping (could also have done a full memset
			// but should be faster that way)
			for (size_t i = 0 ; i < size ; i+= PAGE_SIZE)
				((char*)res)[0] = 0;
		#endif
	} else {
		res = ::mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_ANON|MAP_PRIVATE,0,0);
	}
	if (res == MAP_FAILED)
		return NULL;
	else
		return res;
}

/*******************  FUNCTION  *********************/
void OSUnix::munmap(void* ptr, size_t size)
{
	::munmap(ptr,size);
}

}

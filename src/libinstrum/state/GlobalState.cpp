/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/state/GlobalState.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
*    AUTHOR   : Sébastien Valat - 2026
***********************************************************/

/**********************************************************/
#include <pthread.h>
#include "GlobalState.hpp"
#include "portability/Visibility.hpp"
#include "injectors/InjectPythonInit.hpp"
#include "LazyEnv.hpp"
#include "ReentranceGuard.hpp"
#include "common/Helpers.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
#define GBL_STATE_INIT {ALLOC_WRAP_NOT_READY,MALT_STATIC_MUTEX_INIT,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, NULL, NULL};
const ThreadLocalState TLS_STATE_INIT = {NULL,ALLOC_WRAP_NOT_READY,false, false};

/**********************************************************/
/** Store the global state of allocator wrapper. **/
AllocWrapperGlobal gblState = GBL_STATE_INIT;
/** Store the per-thread state of allocator wrapper. **/
DLL_PUBLIC __thread ThreadLocalState tlsState = TLS_STATE_INIT;
/** Temporary buffer to return on first realloc used by dlsym and split infinit call loops. **/
char gblCallocIniBuffer[4096];

/**********************************************************/
/**
 * Check the stack mode to use by reading MALT_STACK environnement variable.
 * If MALT_STACK does not exist it use the value from config file (stack:mode).
 * 
 * Expected values are :
 *    - 'backtrace'  : use backtrace in malloc/calloc... calls to get the call stack.
 *    - 'libunwind'  : similar to backtrace but use libunwind instead of the glibc implementation.
 *    - 'enter-exit' : follow the call of each function to continuously follow the stack status 
 *                     thanks to -finstrument-function flag of GCC/CLANG/ICC.
**/
static StackMode getStackMode(Options & options)
{
	//default values
	const char * mode = getenv("MALT_STACK");
	StackMode ret = MALT::STACK_MODE_BACKTRACE;
	
	//if not env use config file
	if (mode != NULL)
		setByString(options.stack.mode, mode);
	
	//ok done
	return ret;
}

/**********************************************************/
void maltDumpOnEvent(void)
{
	//stop instr & dump
	if (gblState.status == ALLOC_WRAP_READY)
	{
		gblState.status = ALLOC_WRAP_FINISH;
		gblState.profiler->onExit();

		//TODO restore to pursue profiling
	}
}

/**********************************************************/
static void maltSigKillHandler(int s)
{
	fprintf(stderr,"MALT: Capture signal KILL, dump profile and exit.");
	exit(1);
}

/**********************************************************/
static void maltSigHandler(int signum)
{
	//twik print
	std::string name = OS::getSignalName(signum);

	//print
	fprintf(stderr,"MALT: Capture signal %s, dump profile and continue without profiling.\n", name.c_str());

	//dump
	maltDumpOnEvent();
}

/**********************************************************/
void maltForkHandlerPrepare(void)
{
	//fprintf(stderr, "MALT: PREPARE_FORK\n");
}

/**********************************************************/
void maltForkHandlerChild(void)
{
	//fprintf(stderr, "MALT: FORK_DONE_NOW_IN_CHILD\n");
}

/**********************************************************/
void maltForkHandlerParent(void)
{
	//fprintf(stderr, "MALT: FORK_DONE_NOW_IN_PARENT\n");
}

/**********************************************************/
void maltRegisterPthreadAtFork(void)
{
	//pthread_atfork(maltForkHandlerPrepare, maltForkHandlerParent, maltForkHandlerChild);
}

/**********************************************************/
void maltSetupSigHandler(const Options & options)
{
	//get list
	std::istringstream split(options.dump.onSignal);
	std::vector<std::string> tokens;
	for (std::string each; std::getline(split, each, ','); tokens.push_back(each));

	//loop on all
	for (std::vector<std::string>::const_iterator it = tokens.begin() ; it != tokens.end() ; ++it)
	{
		//debug
		if (gblOptions->output.verbosity != MALT_VERBOSITY_SILENT)
			fprintf(stderr, "MALT: will dump on signal %s (caution, can do only one dump, bugfix in dev !)\n", it->c_str());
		
		//setup
		if (*it == "help" || *it == "avail")
			OS::printAvailSigs();
		else if (*it == "SIGINT")
			OS::setSigHandler(maltSigKillHandler,SIGINT);//TODO check if we keep this !
		else
			OS::setSigHandler(maltSigHandler,*it);
	}
}

/**********************************************************/
ThreadLocalState * maltGetLocalState()
{
	//need init global
	if (gblState.status == ALLOC_WRAP_NOT_READY)
		gblState.init();

	//in case of reentrance we skip
	if (gblState.status != ALLOC_WRAP_READY)
		return nullptr;

	//get TLS
	ThreadLocalState * tls = (ThreadLocalState*)pthread_getspecific(gblState.tlsKey);

	//ok
	return tls;
	//return &tlsState;
}

/**********************************************************/
void * maltDumpAfterSecondsThreadMain(void * arg)
{
	//extract
	int secs = (size_t)arg;

	//we will need it latter to onExit
	maltGetLocalState()->init();

	//wait
	sleep(secs);

	//dump
	maltDumpOnEvent();

	//done
	return NULL;
}

/**********************************************************/
void maltDumpAfterSecondsThread(const Options & options)
{
	//extract
	size_t secs = options.dump.afterSeconds;

	//check
	if (secs == 0)
		return;
	
	//spawn thread (c++11 style)
	/*std::thread th([secs] { 
		maltDumpAfterSecondsThreadMain();
	});*/

	//detach (c+11 style)
	//th.detach();

	//spawn threads
	pthread_t th;
	int rc = pthread_create(&th, NULL, maltDumpAfterSecondsThreadMain, (void*)secs);
	assumeArg(rc == 0, "Fail to start sleep thread for timeout dump: %1").argStrErrno().end();
}

/**********************************************************/
/**
 * Function in charge of the lazy initialization of the global state.
 * It take care of multiple and parallel calls in case of use in multi-thread context.
**/
void AllocWrapperGlobal::init(void )
{
	//skip if filtered exe
	bool skip = false;

	//secure in case of first call in threads
	gblState.lock.lock();

	if (gblState.status == ALLOC_WRAP_NOT_READY)
	{
		//update state
		gblState.status = ALLOC_WRAP_INIT_SYM;
		
		//search addresses for base mmap system layer
		gblState.mmapFuncs.mmap = (MmapFuncPtr)dlsym(RTLD_NEXT,"mmap");
		gblState.mmapFuncs.munmap = (MunmapFuncPtr)dlsym(RTLD_NEXT,"munmap");
		gblState.mmapFuncs.mremap = (MremapFuncPtr)dlsym(RTLD_NEXT,"mremap");

		//search addresses for C allocator
		gblState.allocFuncs.malloc = (MallocFuncPtr)dlsym(RTLD_NEXT,"malloc");
		gblState.allocFuncs.free = (FreeFuncPtr)dlsym(RTLD_NEXT,"free");
		gblState.allocFuncs.calloc = (CallocFuncPtr)dlsym(RTLD_NEXT,"calloc");
		gblState.allocFuncs.realloc = (ReallocFuncPtr)dlsym(RTLD_NEXT,"realloc");
		gblState.allocFuncs.posix_memalign = (PosixMemalignFuncPtr)dlsym(RTLD_NEXT,"posix_memalign");
		gblState.allocFuncs.aligned_alloc = (AlignedAllocFuncPtr)dlsym(RTLD_NEXT,"aligned_alloc");
		gblState.allocFuncs.valloc = (VallocFuncPtr)dlsym(RTLD_NEXT,"valloc");
		gblState.allocFuncs.memalign = (MemalignFuncPtr)dlsym(RTLD_NEXT,"memalign");
		gblState.allocFuncs.pvalloc = (PVallocFuncPtr)dlsym(RTLD_NEXT,"pvalloc");

		//search addresses for GPU allocators
		gblState.gpuFuncs.pgiUaccCudaAlloc = (PgiUaccCudaAllocPtr)dlsym(RTLD_NEXT,"__pgi_uacc_cuda_alloc");
		gblState.gpuFuncs.pgiUaccCudaFree = (PgiUaccCudaFreePtr)dlsym(RTLD_NEXT,"__pgi_uacc_cuda_free");

		//init profiler
		gblState.status = ALLOC_WRAP_INIT_PROFILER;

		//load extended 
		malt_wrap_extended_symbols();
		
		//init internal alloc
		initInternalAlloc(true);

		//init TLS
		pthread_key_create(&this->tlsKey, nullptr);

		//allocate the current one
		ThreadLocalState * firstLocalState = (ThreadLocalState*)MALT_MALLOC(sizeof(ThreadLocalState));
		assert(firstLocalState != nullptr);
		*firstLocalState = TLS_STATE_INIT;
		pthread_setspecific(this->tlsKey, firstLocalState);

		//load options
		gblState.options = &(initGlobalOptions());
		const char * configFile = getenv("MALT_CONFIG");
		if (configFile != NULL)
			gblState.options->loadFromFile(configFile);
		const char * envOptions = getenv("MALT_OPTIONS");
		if (envOptions != NULL)
			gblState.options->loadFromString(envOptions);
		
		//adapt stack skip mode (to adapt malt to compiler inlinling)
		BacktraceStack testStack;
		int stackDelta = testStack.getBactraceSkipOptimDelta();
		gblState.options->stack.skip += stackDelta;
		
		//extract stack mode and update options for libunwind if required
		StackMode mode = getStackMode(*gblState.options);
		
		//ok do it
		void * profilerMem = MALT_MALLOC(sizeof(AllocStackProfiler));
		gblState.profiler = new (profilerMem) AllocStackProfiler(*gblState.options,mode,true);
		gblState.profiler->setRealMallocAddr(gblState.allocFuncs.malloc);

		//filter exe
		if ((gblState.options->filter.exe.empty() == false && OS::getExeName() != gblState.options->filter.exe) || OS::getExeName() == "addr2line")
		{
			if (gblOptions->output.verbosity >= MALT_VERBOSITY_DEFAULT)
				fprintf(stderr,"MALT: skip %s != %s\n",OS::getExeName().c_str(),gblState.options->filter.exe.c_str());
			skip = true;
		}
		
		//print info
		if (gblOptions->output.verbosity >= MALT_VERBOSITY_DEFAULT && !skip)
			fprintf(stderr,"MALT: Start memory instrumentation of %s - %d by library override.\n",OS::getExeName().c_str(),OS::getPID());
		
		//disable childs
		if (gblOptions->filter.childs == false)
			unsetenv("LD_PRELOAD");

		//register on exit
		//TODO remove when ensure that the attribute method work
		//This line tend to create deadlock due to reentrance for some libs (openmpi for example)
		//atexit(AllocWrapperGlobal::onExit);
		
		//register sigkill handler & dump sleep
		maltSetupSigHandler(*gblOptions);
		maltDumpAfterSecondsThread(*gblOptions);

		//pthread at fork handling
		maltRegisterPthreadAtFork();

		//final state
		gblState.status = ALLOC_WRAP_READY;

		//skip
		if (skip)
			gblState.status = ALLOC_WRAP_FINISH;
	}

	//load python
	{
		LazyEnv env;
		ReentranceGuard guard(env);
		MALT::gblHavePython = MALT::PyLazyInterfaceInit();
	}

	//secure in case of first call in threads
	gblState.lock.unlock();
}

/**********************************************************/
/**
 * Function to register into on_exit() one. It permit to cleanup memory and to flush
 * the extracted profile at the end of the execution.
 *
 * @param status Exit status of the application (return value of main()).
 * @param arg Unused argument, will be NULL.
**/
//We use destructor attribute to register and provide a small value for optional
//priority arg, so it might be the last library to destruct to capture as much
//free() as possible for the leak tracking.
void AllocWrapperGlobal::onExit(void)
{
	if (gblState.status == ALLOC_WRAP_READY)
	{
		gblState.status = ALLOC_WRAP_FINISH;
		gblState.profiler->onExit();
		gblState.profiler->~AllocStackProfiler();
		MALT_FREE(gblState.profiler);
		gblState.profiler = nullptr;
	} else {
		gblState.status = ALLOC_WRAP_FINISH;
	}
}

/**********************************************************/
/**
 * Function used on lazy init of the TLS variable, it will be called on first use of the struct.
**/
void ThreadLocalState::init(void)
{
	//check errors
	assert(this->profiler == NULL);
	
	//mark as init to authorize but in use to authorise malloc without intrum and cut infinit call loops
	this->status = ALLOC_WRAP_INIT_PROFILER;

	//check init
	if (gblState.status == ALLOC_WRAP_NOT_READY)
		gblState.init();
	
	//create the local chain
	this->profiler = gblState.profiler->createLocalStackProfiler();
	
	//mark ready
	if (gblOptions->filter.enabled)
		this->status = ALLOC_WRAP_READY;
	else
		this->status = ALLOC_WRAP_DISABLED;
}

/**********************************************************/
void malt_wrap_extended_symbols(void)
{
	//default impl to be overriden by LD_PRELOAD for custom symbols
}

/**********************************************************/
void globalResetForTests(void)
{
	gblState = GBL_STATE_INIT;
	tlsState = TLS_STATE_INIT;
	gblOptions = NULL;
	gblState.init();
}

/**********************************************************/
void globalDump(void)
{
	MALT::gblState.onExit();
}

/**********************************************************/
void initMpiRankFilter(void)
{
	//lazytrigger check
	static bool gblMpiRankCheckDone = false;
	if (gblMpiRankCheckDone == false && Helpers::fileIdIsRank()) {
		//no filter
		if (gblState.options->filter.ranks.empty()) {
			gblMpiRankCheckDone = true;
		} else if (Helpers::getFileId() != static_cast<int>(OS::getPID())) {
			//split & convert to numbers
			IntSet whiteList = Helpers::rankStrToIntSet(gblState.options->filter.ranks);

			//filter rank
			if (whiteList.find(Helpers::getFileId()) != whiteList.end())
				fprintf(stderr,"MALT: instrument only MPI rank %d being in : %s\n",Helpers::getFileId(), gblState.options->filter.ranks.c_str());
			else
				gblState.status = ALLOC_WRAP_FINISH;
			
			//do not lazy check anymore
			gblMpiRankCheckDone = true;
		}
	}
}

}

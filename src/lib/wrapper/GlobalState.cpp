/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : ./src/lib/wrapper/GlobalState.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include "GlobalState.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
#define GBL_STATE_INIT {ALLOC_WRAP_NOT_READY,MALT_STATIC_MUTEX_INIT,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
#define TLS_STATE_INIT {NULL,ALLOC_WRAP_NOT_READY}
/**********************************************************/
/** Store the global state of allocator wrapper. **/
AllocWrapperGlobal gblState = GBL_STATE_INIT;
/** Store the per-thread state of allocator wrapper. **/
__thread ThreadLocalState tlsState = TLS_STATE_INIT;
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
	if (mode == NULL)
		mode = options.stackMode.c_str();

	//switches
	if (mode == NULL)
	{
		ret = STACK_MODE_BACKTRACE;
	} else if (strcmp(mode,"backtrace") == 0 || strcmp(mode,"") == 0) {
		ret = STACK_MODE_BACKTRACE;
	} else if (strcmp(mode,"libunwind") == 0 || strcmp(mode,"") == 0) {
		ret = STACK_MODE_BACKTRACE;
		options.stackLibunwind = true;
	} else if (strcmp(mode,"enter-exit") == 0) {
		ret = STACK_MODE_ENTER_EXIT_FUNC;
	} else {
		MALT_FATAL_ARG("Invalid mode in MALT_STACK environnement variable : '%1'! Supported : backtrace | enter-exit.").arg(mode).end();
	}
	
	//ok done
	return ret;
}

/**********************************************************/
static void maltDumpOnEvent(void)
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
void maltSetupSigHandler(const Options & options)
{
	//get list
	std::istringstream split(options.dumpOnSignal);
	std::vector<std::string> tokens;
	for (std::string each; std::getline(split, each, ','); tokens.push_back(each));

	//loop on all
	for (std::vector<std::string>::const_iterator it = tokens.begin() ; it != tokens.end() ; ++it)
	{
		//debug
		if (gblOptions->outputVerbosity != MALT_VERBOSITY_SILENT)
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
void * maltDumpAfterSecondsThreadMain(void * arg)
{
	//extract
	int secs = (size_t)arg;

	//we will need it latter to onExit
	tlsState.init();

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
	size_t secs = options.dumpAfterSeconds;

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
extern "C" {

/**********************************************************/
int maltInitStatus(void)
{
	return 1;
}
	
void maltEnable(void)
{
	/*get addr localy to avoid to read the TLS every time*/
	ThreadLocalState & localState = tlsState;

	/*check init */ 
	if ( localState.status == ALLOC_WRAP_NOT_READY )
		localState.init();
	
	if(localState.status == ALLOC_WRAP_DISABLED)
		localState.status = ALLOC_WRAP_READY;
}

/**********************************************************/
void maltDisable(void)
{
	/*get addr localy to avoid to read the TLS every time*/
	ThreadLocalState & localState = tlsState;

	/*check init */ 
	if ( localState.status == ALLOC_WRAP_NOT_READY )
		localState.init();
	
	if(localState.status == ALLOC_WRAP_READY)
		localState.status = ALLOC_WRAP_DISABLED;
}
	
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

		//init profiler
		gblState.status = ALLOC_WRAP_INIT_PROFILER;

		//load extended 
		malt_wrap_extended_symbols();
		
		//init internal alloc
		initInternalAlloc(true);

		//load options
		gblState.options = &initGlobalOptions();
		const char * configFile = getenv("MALT_CONFIG");
		if (configFile != NULL)
			gblState.options->loadFromFile(configFile);
		const char * envOptions = getenv("MALT_OPTIONS");
		if (envOptions != NULL)
			gblState.options->loadFromString(envOptions);
		
		//adapt stack skip mode (to adapt malt to compiler inlinling)
		BacktraceStack testStack;
		int stackDelta = testStack.getBactraceSkipOptimDelta();
		gblState.options->stackSkip += stackDelta;
		
		//extract stack mode and update options for libunwind if required
		StackMode mode = getStackMode(*gblState.options);
		
		//ok do it
		gblState.profiler = new AllocStackProfiler(*gblState.options,mode,true);
		gblState.profiler->setRealMallocAddr(gblState.allocFuncs.malloc);

		//filter exe
		if (gblState.options->exe.empty() == false && OS::getExeName() != gblState.options->exe)
		{
			fprintf(stderr,"MALT: skip %s != %s\n",OS::getExeName().c_str(),gblState.options->exe.c_str());
			skip = true;
		}
		
		//print info
		if (gblOptions->outputVerbosity >= MALT_VERBOSITY_DEFAULT && !skip)
			fprintf(stderr,"MALT: Start memory instrumentation of %s - %d by library override.\n",OS::getExeName().c_str(),OS::getPID());
		
		//disable childs
		if (gblOptions->childs == false)
			unsetenv("LD_PRELOAD");

		//register on exit
		//TODO remove when ensure that the attribute method work
		//This line tend to create deadlock due to reentrance for some libs (openmpi for example)
		//atexit(AllocWrapperGlobal::onExit);
		
		//register sigkill handler & dump sleep
		maltSetupSigHandler(*gblOptions);
		maltDumpAfterSecondsThread(*gblOptions);

		//final state
		gblState.status = ALLOC_WRAP_READY;

		//skip
		if (skip)
			gblState.status = ALLOC_WRAP_FINISH;
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
		delete gblState.profiler;
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
	assert(tlsState.profiler == NULL);
	
	//mark as init to authorize but in use to authorise malloc without intrum and cut infinit call loops
	tlsState.status = ALLOC_WRAP_INIT_PROFILER;

	//check init
	if (gblState.status == ALLOC_WRAP_NOT_READY)
		gblState.init();
	
	//create the local chain
	tlsState.profiler = gblState.profiler->createLocalStackProfiler(true);
	
	//mark ready
	if (gblOptions->enabled)
		tlsState.status = ALLOC_WRAP_READY;
	else
		tlsState.status = ALLOC_WRAP_DISABLED;
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
}

}

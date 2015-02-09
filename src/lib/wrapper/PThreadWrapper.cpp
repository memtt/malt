/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//standard
#include "PThreadWrapper.hpp"
#include <cstdio>
#include <cassert>
#include <portability/OS.hpp>

namespace MATT
{

/********************* GLOBALS **********************/
/**
 * Static instance of thread tracker.
**/
static PthreadWrapperData gblWrapperData = {NULL,NULL};

/*******************  FUNCTION  *********************/
/**
 * Function called on thread exit via pthread_key system.
 * Argument is not used, only defined for signature compatibility.
**/
void pthreadWrapperOnExit(void *)
{
	if (gblWrapperData.threadHooks != NULL)
		gblWrapperData.threadHooks->onThreadExit();
}

/*******************  FUNCTION  *********************/
/**
 * Function to be used as wrapper in pthread_create to capture the thread init, update
 * counters and call the real user function.
 * @param arg Muse get a pointer to newly allocated PThreadWrapperArg structure.
**/
void * pthreadWrapperStartRoutine(void * arg)
{
	//send event
	if (gblWrapperData.threadHooks != NULL)
		gblWrapperData.threadHooks->onThreadCreate();	

	//setup the key to get destructor call on thread exit (capture function finish or pthread_exit)
	pthread_setspecific(gblWrapperData.key, (void*)0x1);
	
	//fprintf(stderr,"Create thread : %d / %d !\n",gblPThreadWrapperData.threadCount,gblPThreadWrapperData.maxThreadCount);
	
	//run child
	PThreadWrapperArg * subarg = (PThreadWrapperArg *)arg;
	void * res = subarg->routine(subarg->arg);
	
	//delete
	delete subarg;
	
	//return
	return res;
}

}

/*******************  FUNCTION  *********************/
/**
 * Wrapper of thread_create method.
**/
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,void *(*start_routine) (void *), void *arg)
{
	//init
	if (MATT::gblWrapperData.pthread_create == NULL)
	{
		MATT::gblWrapperData.threadHooks = MATT::threadHookInit();
		MATT::gblWrapperData.pthread_create = MATT::OS::dlsymNext<MATT::PthreadCreateFuncPtr>("pthread_create");
		pthread_key_create(&MATT::gblWrapperData.key,MATT::pthreadWrapperOnExit);
	}
	
	//prepare args
	MATT::PThreadWrapperArg * subarg = new MATT::PThreadWrapperArg;
	subarg->arg = arg;
	subarg->routine = start_routine;

	//call
	return MATT::gblWrapperData.pthread_create(thread,attr,MATT::pthreadWrapperStartRoutine,subarg);
}


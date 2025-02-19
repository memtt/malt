/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : ./src/lib/wrapper/GlobalState.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#ifndef MALT_LAZY_ENV_HPP
#define MALT_LAZY_ENV_HPP

/**********************************************************/
#include "GlobalState.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
class LazyEnv
{
	public:
		inline LazyEnv();
		inline AllocStackProfiler & getGlobalProfiler();
		inline LocalAllocStackProfiler & getLocalProfiler();
		inline AllocWrapperGlobal & getGlobalState();
		inline ThreadLocalState & getLocalState();
		inline bool isEnterExit() const;
		inline bool isPythonEnterExit() const;
		inline bool isMaltReady() const;
		inline bool markInstrumented();
		inline void endInstrumentation(bool oldStatus);
	private:
		ThreadLocalState & threadLocalState;
		bool enterExit;
};

/**********************************************************/
LazyEnv::LazyEnv()
        :threadLocalState(tlsState)
{
	/*check init */
	if ( this->threadLocalState.status == ALLOC_WRAP_NOT_READY )
		this->threadLocalState.init();

	this->enterExit = true;
	if (threadLocalState.profiler != NULL){
		this->enterExit = this->threadLocalState.profiler->isEnterExit();
	}
}

/**********************************************************/
AllocStackProfiler & LazyEnv::getGlobalProfiler()
{
	return *(gblState.profiler);
}

/**********************************************************/
LocalAllocStackProfiler & LazyEnv::getLocalProfiler()
{
	return *(this->threadLocalState.profiler);
}

/**********************************************************/
AllocWrapperGlobal & LazyEnv::getGlobalState()
{
	return gblState;
}

/**********************************************************/
ThreadLocalState & LazyEnv::getLocalState()
{
	return this->threadLocalState;
}

/**********************************************************/
bool LazyEnv::isEnterExit() const
{
	return enterExit;
}

/**********************************************************/
bool LazyEnv::markInstrumented()
{
	bool oldStatus = this->threadLocalState.inMalt;
	this->threadLocalState.inMalt = true;
	return oldStatus;
}

/**********************************************************/
void LazyEnv::endInstrumentation(bool oldStatus)
{
	//cannot have false / false
	assert(this->threadLocalState.inMalt != oldStatus || oldStatus == true);
	this->threadLocalState.inMalt = oldStatus;
}

/**********************************************************/
bool LazyEnv::isMaltReady() const
{
	return (gblState.status == ALLOC_WRAP_READY && this->threadLocalState.status == ALLOC_WRAP_READY);
}

}

#endif //MALT_LAZY_ENV_HPP

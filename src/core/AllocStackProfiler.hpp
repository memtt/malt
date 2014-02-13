/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_ALLOC_STACK_PROGILER_HPP
#define MATT_ALLOC_STACK_PROGILER_HPP

/********************  HEADERS  *********************/
//standard
#include <cstdlib>
//intenrals
#include <common/Options.hpp>
#include <portability/Mutex.hpp>
#include "SegmentTracker.hpp"
#include "SimpleStackTracer.hpp"
#include "EnterExitCallStack.hpp"
#include "ProfiledValue.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/********************  ENUM  ************************/
enum StackMode
{
	STACK_MODE_BACKTRACE,
	STACK_MODE_ENTER_EXIT_FUNC,
	STACK_MODE_USER
};

/*********************  CLASS  **********************/
class AllocStackProfiler
{
	public:
		AllocStackProfiler(const Options & option, StackMode mode = STACK_MODE_BACKTRACE,bool threadSafe = false);
		void onMalloc(void * ptr,size_t size,Stack * userStack = NULL);
		void onCalloc(void * ptr,size_t nmemb,size_t size,Stack * userStack = NULL);
		void onPrepareRealloc(void * oldPtr,Stack * userStack = NULL);
		void onRealloc(void * oldPtr,void * ptr,size_t newSize,Stack * userStack = NULL);
		void onFree(void * ptr,Stack * userStack = NULL);
		void onExit(void);
		void onEnterFunction(void * funcAddr);
		void onExitFunction(void * funcAddr);
	public:
		friend void convertToJson(htopml::JsonState& json, const AllocStackProfiler& value);
	private:
		SimpleCallStackNode * getStackNode(int skipDepth,ssize_t delta,Stack * userStack = NULL);
		SimpleCallStackNode * onAllocEvent(void * ptr,size_t size, int skipDepth,Stack * userStack = NULL,SimpleCallStackNode * callStackNode = NULL,bool doLock = true);
		SimpleCallStackNode * onFreeEvent(void* ptr, int skipDepth, Stack* userStack = NULL, SimpleCallStackNode* callStackNode = NULL, bool doLock = true);
	private:
		SimpleStackTracer stackTracer;
		SegmentTracker segTracker;
		BacktraceCallStack stack;
		EnterExitCallStack exStack;
		ProfiledValue requestedMem;
		ProfiledValue physicalMem;
		ProfiledValue virtualMem;
		ProfiledValue internalMem;
		ProfiledValue segments;
		StackMode mode;
		Mutex lock;
		bool threadSafe;
		Options options;
};

}

#endif //MATT_ALLOC_STACK_PROGILER_HPP

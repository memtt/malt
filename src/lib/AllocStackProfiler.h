#ifndef ALLOC_STATCK_PROFILER_H
#define ALLOC_STATCK_PROFILER_H

/********************  HEADERS  *********************/
#include <cstdlib>
#include <portability/Mutex.h>
#include "SegmentTracker.h"
#include "SimpleStackTracer.h"
#include "EnterExitCallStack.h"

/********************  ENUM  ************************/
enum StackMode
{
	STACK_MODE_BACKTRACE,
	STACK_MODE_ENTER_EXIT_FUNC,
	STACK_MODE_USER,
};

/*********************  CLASS  **********************/
class AllocStackProfiler
{
	public:
		AllocStackProfiler(StackMode mode = STACK_MODE_BACKTRACE,bool threadSafe = false);
		void onMalloc(void * ptr,size_t size,Stack * userStack = NULL);
		void onCalloc(void * ptr,size_t nmemb,size_t size,Stack * userStack = NULL);
		void onPrepareRealloc(void * oldPtr,Stack * userStack = NULL);
		void onRealloc(void * oldPtr,void * ptr,size_t newSize,Stack * userStack = NULL);
		void onFree(void * ptr,Stack * userStack = NULL);
		void onExit(void);
		void onEnterFunction(void * funcAddr);
		void onExitFunction(void * funcAddr);
	private:
		SimpleCallStackNode * getStackNode(int skipDepth,ssize_t delta,Stack * userStack = NULL);
		SimpleCallStackNode * onAllocEvent(void * ptr,size_t size, int skipDepth,Stack * userStack = NULL,SimpleCallStackNode * callStackNode = NULL,bool doLock = true);
		SimpleCallStackNode * onFreeEvent(void* ptr, int skipDepth, Stack* userStack = NULL, SimpleCallStackNode* callStackNode = NULL, bool doLock = true);
	private:
		SimpleStackTracer stackTracer;
		SegmentTracker segTracer;
		BacktraceCallStack stack;
		EnterExitCallStack exStack;
		StackMode mode;
		Mutex lock;
		bool threadSafe;
};

#endif //ALLOC_STATCK_PROFILER_H

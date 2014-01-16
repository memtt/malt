#ifndef ALLOC_STATCK_PROFILER_H
#define ALLOC_STATCK_PROFILER_H

/********************  HEADERS  *********************/
#include "SimpleStackTracer.h"
#include "EnterExitCallStack.h"
#include <portability/Mutex.h>

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
		void countCalls(int skipDepth,ssize_t delta,Stack * userStack = NULL);
	private:
		SimpleStackTracer tracer;
		BacktraceCallStack stack;
		EnterExitCallStack exStack;
		StackMode mode;
		Mutex lock;
		bool threadSafe;
};

#endif //ALLOC_STATCK_PROFILER_H

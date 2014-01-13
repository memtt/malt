#ifndef ALLOC_STATCK_PROFILER_H
#define ALLOC_STATCK_PROFILER_H

/********************  HEADERS  *********************/
#include "SimpleStackTracer.h"
#include "EnterExitCallStack.h"

/********************  ENUM  ************************/
enum StackMode
{
	STACK_MODE_BACKTRACE,
	STACK_MODE_ENTER_EXIT_FUNC,
};

/*********************  CLASS  **********************/
class AllocStackProfiler
{
	public:
		AllocStackProfiler(StackMode mode = STACK_MODE_BACKTRACE);
		void onMalloc(void * ptr,size_t size);
		void onCalloc(void * ptr,size_t nmemb,size_t size);
		void onPrepareRealloc(void * oldPtr);
		void onRealloc(void * oldPtr,void * ptr,size_t newSize);
		void onFree(void * ptr);
		void onExit(void);
		void onEnterFunction(void * funcAddr);
		void onExitFunction(void * funcAddr);
	private:
		void countCalls(int skipDepth,ssize_t delta);
	private:
		SimpleStackTracer tracer;
		BacktraceCallStack stack;
		EnterExitCallStack exStack;
		StackMode mode;
};

#endif //ALLOC_STATCK_PROFILER_H

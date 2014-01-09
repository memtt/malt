#ifndef ALLOC_STATCK_PROFILER_H
#define ALLOC_STATCK_PROFILER_H

/********************  HEADERS  *********************/
#include "SimpleStackTracer.h"
#include "EnterExitCallStack.h"

/*********************  CLASS  **********************/
class AllocStackProfiler
{
	public:
		AllocStackProfiler(void);
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
		SimpleCallStack stack;
		EnterExitCallStack exStack;
};

#endif //ALLOC_STATCK_PROFILER_H

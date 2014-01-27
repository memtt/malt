#ifndef ATT_ALLOC_STACK_PROGILER_HPP
#define ATT_ALLOC_STACK_PROGILER_HPP

/********************  HEADERS  *********************/
#include <cstdlib>
#include <portability/Mutex.hpp>
#include "SegmentTracker.hpp"
#include "SimpleStackTracer.hpp"
#include "EnterExitCallStack.hpp"
#include "ProfiledValue.hpp"
#include <common/Options.hpp>

/*******************  NAMESPACE  ********************/
namespace ATT
{

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
		friend void typeToJson(htopml::JsonState& json, std::ostream& stream, const AllocStackProfiler& value);
	private:
		SimpleCallStackNode * getStackNode(int skipDepth,ssize_t delta,Stack * userStack = NULL);
		SimpleCallStackNode * onAllocEvent(void * ptr,size_t size, int skipDepth,Stack * userStack = NULL,SimpleCallStackNode * callStackNode = NULL,bool doLock = true);
		SimpleCallStackNode * onFreeEvent(void* ptr, int skipDepth, Stack* userStack = NULL, SimpleCallStackNode* callStackNode = NULL, bool doLock = true);
	private:
		SimpleStackTracer stackTracer;
		SegmentTracker segTracer;
		BacktraceCallStack stack;
		EnterExitCallStack exStack;
		ProfiledValue requestedMem;
		StackMode mode;
		ATT::Mutex lock;
		bool threadSafe;
		Options options;
};

};

#endif //ATT_ALLOC_STACK_PROGILER_HPP

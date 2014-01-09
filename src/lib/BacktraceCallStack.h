#ifndef BACKTRACE_BACKTRACE_H
#define BACKTRACE_BACKTRACE_H

/********************  HEADERS  *********************/
#include "Stack.h"
#include "FuncNameDic.h"
#include "CallStackInfo.h"

/*********************  CLASS  **********************/
class BacktraceCallStack : public Stack
{
	public:
		BacktraceCallStack(void);
		void loadCurrentStack(void);
};

#endif //BACKTRACE_BACKTRACE_H

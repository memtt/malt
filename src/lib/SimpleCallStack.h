#ifndef SIMPLE_BACKTRACE_H
#define SIMPLE_BACKTRACE_H

/********************  HEADERS  *********************/
#include "Stack.h"
#include "FuncNameDic.h"
#include "CallStackInfo.h"

/*********************  CLASS  **********************/
class SimpleCallStack : public Stack
{
	public:
		SimpleCallStack(void);
		void loadCurrentStack(void);
};

#endif //SIMPLE_BACKTRACE_H

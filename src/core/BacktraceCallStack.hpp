#ifndef ATT_BACKTRACE_BACKTRACE_HPP
#define ATT_BACKTRACE_BACKTRACE_HPP

/********************  HEADERS  *********************/
#include "Stack.h"
#include "FuncNameDic.h"
#include "CallStackInfo.hpp"

/*******************  NAMESPACE  ********************/
namespace ATT
{

/*********************  CLASS  **********************/
class BacktraceCallStack : public Stack
{
	public:
		BacktraceCallStack(void);
		void loadCurrentStack(void);
};

};

#endif //ATT_BACKTRACE_BACKTRACE_HPP

#ifndef ENTER_EXIT_CALL_STACK_H
#define ENTER_EXIT_CALL_STACK_H

/********************  HEADERS  *********************/
#include <ostream>
#include <json/JsonState.h>
#include "Stack.h"

/*********************  CLASS  **********************/
class EnterExitCallStack : public Stack
{
	public:
		EnterExitCallStack(void);
		void enterFunction(void * funcAddr);
		void exitFunction(void * funcAddr);
};

#endif //ENTER_EXIT_CALL_STACK_H

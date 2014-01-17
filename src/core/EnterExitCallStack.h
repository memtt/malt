#ifndef ENTER_EXIT_CALL_STACK_H
#define ENTER_EXIT_CALL_STACK_H

/********************  HEADERS  *********************/
#include <ostream>
#include <cstdlib>
// #include <json/JsonState.h>
#include "Stack.h"

/*******************  FUNCTION  *********************/
namespace htopml
{
	class JsonState;
};

/*********************  CLASS  **********************/
class EnterExitCallStack : public Stack
{
	public:
		EnterExitCallStack(void);
		void enterFunction(void * funcAddr);
		void exitFunction(void * funcAddr);
	private:
		size_t realSize;
};

#endif //ENTER_EXIT_CALL_STACK_H

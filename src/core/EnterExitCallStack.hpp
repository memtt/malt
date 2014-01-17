#ifndef ATT_ENTER_EXIT_CALL_STACK_HPP
#define ATT_ENTER_EXIT_CALL_STACK_HPP

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

/*******************  NAMESPACE  ********************/
namespace ATT
{

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

}

#endif //ATT_ENTER_EXIT_CALL_STACK_HPP

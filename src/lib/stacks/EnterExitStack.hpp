/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_ENTER_EXIT_STACK_HPP
#define MATT_ENTER_EXIT_STACK_HPP

/********************  HEADERS  *********************/
//standard
#include <ostream>
#include <cstdlib>
//internals
#include <stacks/Stack.hpp>

/*******************  FUNCTION  *********************/
namespace htopml
{
	class JsonState;
}

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  CLASS  **********************/
/**
 * Provide a specialized stack to manage the enter-exit mode to track stacks.
 * In this mode, the app need to notify the tool when on entry/exit point of each functions.
 * This way, we can rebuild the stack.
**/
class EnterExitStack : public Stack
{
	public:
		EnterExitStack(void);
		void enterFunction(void * funcAddr);
		void exitFunction(void * funcAddr);
	private:
		size_t realSize;
};

}

#endif //MATT_ENTER_EXIT_STACK_HPP

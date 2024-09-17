/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.2
*    DATE     : 09/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/stacks/EnterExitStack.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

#ifndef MALT_ENTER_EXIT_STACK_HPP
#define MALT_ENTER_EXIT_STACK_HPP

/**********************************************************/
//standard
#include <ostream>
#include <cstdlib>
#include <cassert>
//internals
#include <stacks/Stack.hpp>


/**********************************************************/
#define MALT_MAX_STACK_SIZE (1024*1204)

/**********************************************************/
namespace htopml
{
	class JsonState;
}

/**********************************************************/
namespace MALT
{

/**********************************************************/
/**
 * Provide a specialized stack to manage the enter-exit mode to track stacks.
 * In this mode, the app need to notify the tool when on entry/exit point of each functions.
 * This way, we can rebuild the stack.
**/
class EnterExitStack : public Stack
{
	public:
		EnterExitStack(void);
		inline void enterFunction(void * funcAddr);
		inline void exitFunction(void * funcAddr);
	private:
		size_t realSize;
};

/**********************************************************/
/**
 * Notify function entry point.
 * It may produce a reallocation of the storage segment.
**/
inline void EnterExitStack::enterFunction ( void* funcAddr )
{
	//check default
	if (stack == NULL)
		this->grow();

	//check realSize
	size_t tmp = ++realSize;
	if (tmp > MALT_MAX_STACK_SIZE)
		return;
	
	//check size
	if (size == memSize || stack == NULL)
		this->grow();
		
	//update
	this->stack[size++] = funcAddr;
	
	//errors
	assert(size <= memSize);
}

/**********************************************************/
/**
 * Notify a function exit to remove the last stack entry.
 * It will not free the related memory for future reuse.
**/
inline void EnterExitStack::exitFunction ( void* funcAddr )
{
	assert(size > 0);

	size_t tmp = realSize--;
	if (tmp >MALT_MAX_STACK_SIZE)
		return;

	if (size > 0)
		size--;

	assert(funcAddr == stack[size]);
}

}

#endif //MALT_ENTER_EXIT_STACK_HPP

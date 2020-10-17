/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALTV2_ENTER_EXIT_STACK_HPP
#define MALTV2_ENTER_EXIT_STACK_HPP

/********************  HEADERS  *********************/
//standard
#include <ostream>
#include <cstdlib>
#include <cassert>
//internals
#include "Stack.hpp"


/********************  MACROS  **********************/
#define MALT_MAX_STACK_SIZE (1024*1204)

/*******************  FUNCTION  *********************/
namespace htopml
{
	class JsonState;
}

/*******************  NAMESPACE  ********************/
namespace MALTV2
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
		inline void enterFunction(void * funcAddr);
		inline void exitFunction(void * funcAddr);
	private:
		size_t realSize;
};

/*******************  FUNCTION  *********************/
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

/*******************  FUNCTION  *********************/
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

#endif //MALTV2_ENTER_EXIT_STACK_HPP

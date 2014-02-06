/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_ENTER_EXIT_CALL_STACK_HPP
#define MATT_ENTER_EXIT_CALL_STACK_HPP

/********************  HEADERS  *********************/
//standard
#include <ostream>
#include <cstdlib>
//internals
#include "Stack.h"

/*******************  FUNCTION  *********************/
namespace htopml
{
	class JsonState;
}

/*******************  NAMESPACE  ********************/
namespace MATT
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

#endif //MATT_ENTER_EXIT_CALL_STACK_HPP

/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_STACK_TREE_HPP
#define MATT_STACK_TREE_HPP
#include "Stack.hpp"

/********************  HEADERS  *********************/

/*******************  NAMESPACE  ********************/
namespace MATT
{

typedef void * StackTreeHandler;

/*********************  CLASS  **********************/
class StackTree
{
	public:
		StackTree(void){};
		~StackTree(void){};
		virtual StackTreeHandler enterFunction(StackTreeHandler handler,void * callsite) = 0;
		virtual StackTreeHandler exitFunction(StackTreeHandler handler,void  * callsite) = 0;
		virtual StackTreeHandler enterThread(void) = 0;
		virtual StackTreeHandler setFromStack(StackTreeHandler handler,const Stack & stack) = 0;
		virtual void exitThread(StackTreeHandler handler) = 0;
	protected:
		virtual void * getData(StackTreeHandler handler) = 0;
		virtual void setData(StackTreeHandler handler,void * data) = 0;
};

}

#endif //MATT_STACK_TREE_HPP

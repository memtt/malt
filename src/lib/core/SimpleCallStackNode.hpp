/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.2
*    DATE     : 09/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/core/SimpleCallStackNode.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

#ifndef MALT_SIMPLE_CALL_STACK_NODE_HPP
#define MALT_SIMPLE_CALL_STACK_NODE_HPP

/**********************************************************/
#include "CallStackInfo.hpp"
#include <stacks/Stack.hpp>

/**********************************************************/
namespace MALT
{

/**********************************************************/
/**
 * @brief Temporary struct to point ressources joining a stack with its profiling infos.
**/
struct MMCallStackNode
{
	/** Constrcutor with default values (NULL). **/
	MMCallStackNode(void) {stack = NULL; infos = NULL;};
	/** 
	 * Constructor with given infos.
	 * @param stack Pointer to the stack to be used as init.
	 * @param infos Pointer to the profiling infos to be used as init.
	**/
	MMCallStackNode(const Stack * stack,CallStackInfo * infos) {this->stack = stack; this->infos = infos;};
	/** Check if valid (pointers are not NULL). **/
	bool valid(void) const {return stack != NULL && infos != NULL;};
	/** Pointer to the stack **/
	const Stack * stack;
	/** Pointer to the profiling infos attached the current stack. **/
	CallStackInfo * infos;
};

/**********************************************************/
/**
 * @brief Define a node joining a stack with its profiling infos.
**/
class SimpleCallStackNode
{
	public:
		SimpleCallStackNode(const Stack & stack, int skipDepth = 0);
		/** @return Reference to the profiling info struct. **/
		CallStackInfo & getInfo(void) {return info;};
		/** @return Reference to the call stack. **/
		Stack & getCallStack(void) { return callStack;};
		/** @return Const reference to the call stack. **/
		const Stack & getCallStack(void) const { return callStack;};
	public:
		friend std::ostream & operator << (std::ostream & out,const SimpleCallStackNode & tracer);
		friend void convertToJson(htopml::JsonState & json, const SimpleCallStackNode & value);
	private:
		//ensure to never copy as private
		SimpleCallStackNode(const SimpleCallStackNode & orig);
		SimpleCallStackNode & operator = (const SimpleCallStackNode & orig);
	private:
		/** Keep track of the call stack. **/
		Stack callStack;
		/** Profiling infos attached to the current call stack. **/
		CallStackInfo info;
};

}

#endif //MALT_SIMPLE_CALL_STACK_NODE_HPP

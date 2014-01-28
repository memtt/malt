/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_SIMPLE_CALL_STACK_NODE_HPP
#define MATT_SIMPLE_CALL_STACK_NODE_HPP

/********************  HEADERS  *********************/
#include "CallStackInfo.hpp"
#include "Stack.h"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  CLASS  **********************/
class SimpleCallStackNode
{
	public:
		SimpleCallStackNode(const Stack & stack);
		CallStackInfo & getInfo(void) {return info;};
		Stack & getCallStack(void) { return callStack;};
	public:
		friend std::ostream & operator << (std::ostream & out,const SimpleCallStackNode & tracer);
		friend void convertToJson(htopml::JsonState & json, const SimpleCallStackNode & value);
	private:
		//ensure to never copy as private
		SimpleCallStackNode(const SimpleCallStackNode & orig);
		SimpleCallStackNode & operator = (const SimpleCallStackNode & orig);
	private:
		Stack callStack;
		CallStackInfo info;
};

}

#endif //MATT_SIMPLE_CALL_STACK_NODE_HPP

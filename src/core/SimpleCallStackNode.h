#ifndef SIMPLE_CALL_STACK_NODE_H
#define SIMPLE_CALL_STACK_NODE_H

/********************  HEADERS  *********************/
#include "CallStackInfo.hpp"
#include "Stack.h"

/*********************  CLASS  **********************/
class SimpleCallStackNode
{
	public:
		SimpleCallStackNode(const Stack & stack);
		ATT::CallStackInfo & getInfo(void) {return info;};
		Stack & getCallStack(void) { return callStack;};
	public:
		friend std::ostream & operator << (std::ostream & out,const SimpleCallStackNode & tracer);
		friend void typeToJson(htopml::JsonState & json,std::ostream& stream, const SimpleCallStackNode & value);
	private:
		//ensure to never copy as private
		SimpleCallStackNode(const SimpleCallStackNode & orig);
		SimpleCallStackNode & operator = (const SimpleCallStackNode & orig);
	private:
		Stack callStack;
		ATT::CallStackInfo info;
};

#endif //SIMPLE_CALL_STACK_NODE_H

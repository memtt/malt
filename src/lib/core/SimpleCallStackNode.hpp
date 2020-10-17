/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_SIMPLE_CALL_STACK_NODE_HPP
#define MALT_SIMPLE_CALL_STACK_NODE_HPP

/********************  HEADERS  *********************/
#include "CallStackInfo.hpp"
#include <stacks/Stack.hpp>

/*******************  NAMESPACE  ********************/
namespace MALT
{

/********************  STRUCT  **********************/
struct MMCallStackNode
{
	MMCallStackNode(void) {stack = NULL; infos = NULL;};
	MMCallStackNode(const Stack * stack,CallStackInfo * infos) {this->stack = stack; this->infos = infos;};
	bool valid(void) const {return stack != NULL && infos != NULL;};
	const Stack * stack;
	CallStackInfo * infos;
};

/*********************  CLASS  **********************/
class SimpleCallStackNode
{
	public:
		SimpleCallStackNode(const Stack & stack, int skipDepth = 0);
		CallStackInfo & getInfo(void) {return info;};
		Stack & getCallStack(void) { return callStack;};
		const Stack & getCallStack(void) const { return callStack;};
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

#endif //MALT_SIMPLE_CALL_STACK_NODE_HPP

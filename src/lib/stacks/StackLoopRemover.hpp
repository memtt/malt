/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/stacks/StackLoopRemover.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2015 - 2024
***********************************************************/

#ifndef MALT_STACK_LOOP_REMOVER_H
#define MALT_STACK_LOOP_REMOVER_H

/**********************************************************/
//internal
#include "Stack.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
#define MALT_STACK_NODE_MAX_LINK 8

/**********************************************************/
class StackLoopNode
{
	public:
		StackLoopNode(void);
		StackLoopNode * getLink(LangAddress targetAddr);
		void addLinkTo(StackLoopNode * target);
		StackLoopNode * followNextUnused(void);
		void reset(void);
		void setAddr(LangAddress addr);
		LangAddress getAddr(void);
	private:
		LangAddress addr;
		StackLoopNode * links[MALT_STACK_NODE_MAX_LINK];
		bool used[MALT_STACK_NODE_MAX_LINK];
};

/**********************************************************/
class StackLoopRemover
{
	public:
		StackLoopRemover(int maxSize);
		~StackLoopRemover(void);
		void removeLoops(Stack & stack);
		void reset(void);
	private:
		StackLoopNode * nodes;
		int maxSize;
		LangAddress* rebuildStack;
};

}

#endif //MALT_STACK_LOOP_REMOVER_H

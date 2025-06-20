/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/stacks/StackLoopRemover.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2015 - 2018
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
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
#define MALT_STACK_NODE_MAX_LINK 32

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

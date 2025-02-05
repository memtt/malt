/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_STACK_LOOP_REMOVER_H
#define MALT_STACK_LOOP_REMOVER_H

/********************  HEADERS  *********************/
//internal
#include "Stack.hpp"

/*******************  NAMESPACE  ********************/
namespace MALT
{

/********************  MACROS  **********************/
#define MALT_STACK_NODE_MAX_LINK 8

/*********************  CLASS  **********************/
class StackLoopNode
{
	public:
		StackLoopNode(void);
		StackLoopNode * getLink(AddressType targetAddr);
		void addLinkTo(StackLoopNode * target);
		StackLoopNode * followNextUnused(void);
		void reset(void);
		void setAddr(AddressType addr);
		AddressType getAddr(void);
	private:
		AddressType addr;
		StackLoopNode * links[MALT_STACK_NODE_MAX_LINK];
		bool used[MALT_STACK_NODE_MAX_LINK];
};

/*********************  CLASS  **********************/
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
		AddressType* rebuildStack;
};

}

#endif //MALT_STACK_LOOP_REMOVER_H

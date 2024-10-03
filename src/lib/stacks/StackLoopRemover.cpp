/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/stacks/StackLoopRemover.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2015 - 2024
*    AUTHOR   : Sébastien Valat (CERN) - 2015
***********************************************************/

/**********************************************************/
//std
#include <cassert>
//internal
#include <common/Debug.hpp>
#include "StackLoopRemover.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
StackLoopNode::StackLoopNode(void )
{
	this->reset();
}

/**********************************************************/
void StackLoopNode::addLinkTo(StackLoopNode * target)
{
	assert(target != NULL);
	for (int i = 0 ; i < MALT_STACK_NODE_MAX_LINK ; i++)
	{
		if (links[i] == NULL)
		{
			links[i] = target;
			return;
		}
	}
	
	//TODO replace by FATAL
	MALT_FATAL_ARG("Not enougth link to unloop stacks, limit is %1, see macro MALT_STACK_NODE_MAX_LINK").arg(MALT_STACK_NODE_MAX_LINK).end();
}

/**********************************************************/
StackLoopNode* StackLoopNode::followNextUnused(void )
{
	for (int i = 0 ; i < MALT_STACK_NODE_MAX_LINK ; i++)
	{
		if (!used[i])
		{
			used[i] = true;
			return links[i];
		}
	}
	return NULL;
}

/**********************************************************/
StackLoopNode* StackLoopNode::getLink(void* targetAddr)
{
	for (int i = 0 ; i < MALT_STACK_NODE_MAX_LINK ; i++)
	{
		if (links[i] == NULL)
			return NULL;
		else if (links[i]->addr == targetAddr)
			return links[i];
	}

	return NULL;
}

/**********************************************************/
void StackLoopNode::setAddr(void* addr)
{
	assert(this->addr == NULL);
	this->addr = addr;
}

/**********************************************************/
void StackLoopNode::reset(void )
{
	this->addr = NULL;
	for (int i = 0 ; i < MALT_STACK_NODE_MAX_LINK ; i++)
	{
		links[i] = NULL;
		used[i] = false;
	}
}

/**********************************************************/
StackLoopRemover::StackLoopRemover(int maxSize)
{
	assert(maxSize > 0);
	this->nodes = new StackLoopNode[maxSize];
	this->rebuildStack = new void*[maxSize];
	this->maxSize = maxSize;
}

/**********************************************************/
StackLoopRemover::~StackLoopRemover(void )
{
	delete [] this->rebuildStack;
	delete [] this->nodes;
}

/**********************************************************/
void* StackLoopNode::getAddr(void )
{
	return addr;
}

/**********************************************************/
void StackLoopRemover::reset(void )
{
	for (int i = 0 ; i < maxSize ; i++)
		nodes[i].reset();
}

/**********************************************************/
void StackLoopRemover::removeLoops(Stack& stack)
{
	//check
	assert(stack.getSize() > 0);
	
	//allocate
	assume(stack.getSize() <= this->maxSize,"Invalid size, not matched with what requested at build time");
	this->reset();
	StackLoopNode * cur = nodes;
	
	//setup first
	cur->setAddr(stack[0]);
	
	//loop on each entry fro stack
	for (int i = 1 ; i < stack.getSize() ; i++)
	{
		//search in links
		StackLoopNode * n = cur->getLink(stack[i]);
		
		//if not found search in existing && add link
		if (n == NULL)
		{
			for (int j = 0 ; j < stack.getSize() ; j++)
			{
				if (nodes[j].getAddr() == stack[i])
				{
					n = &nodes[j];
					cur->addLinkTo(n);
					break;
				}
			}
		}
		
		//if stil null, allocate a new one
		if (n == NULL)
		{
			for (int j = 0 ; j < stack.getSize() ; j++)
			{
				if (nodes[j].getAddr() == NULL)
				{
					n = &nodes[j];
					n->setAddr(stack[i]);
					cur->addLinkTo(n);
					break;
				}
			}
		}
		
		//now can move to it
		assert(n != NULL);
		cur = n;
	}
	
	//now just print by following paths in order and mark them as taken
	int cnt = 0;
	StackLoopNode * n = &nodes[0];
	while (n != NULL)
	{
		//print
		rebuildStack[cnt++] = n->getAddr();
		//search first non used path
		n = n->followNextUnused();
	}
	
	//import
	stack.set(rebuildStack,cnt,STACK_ORDER_ASC);
}

}

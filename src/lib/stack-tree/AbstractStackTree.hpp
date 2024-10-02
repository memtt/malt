/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.3
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/stack-tree/AbstractStackTree.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2024
***********************************************************/

#ifndef MALT_RLOCK_FREE_TREE_HPP
#define MALT_RLOCK_FREE_TREE_HPP

/**********************************************************/
//std
#include <cassert>
#include "StackSTLHashMap.hpp"
#include "RLockFreeTree.hpp"


/**********************************************************/
namespace MALT
{
	
/**********************************************************/
typedef void * StackNodeHanlder;

/**********************************************************/
template <class T>
class AbstractStackTree
{
	public:
		AbstractStackTree(bool useHashMap = true,bool threadSafe = true);
		StackNodeHanlder getNode(const Stack & stack);
		StackNodeHanlder getChild(StackNodeHanlder handler,void * callsite);
		StackNodeHanlder getParent(StackNodeHanlder handler);
		T * getData(StackNodeHanlder handler);
		void toJson(htopml::JsonState & state);
	private:
		bool useHashMap;
		bool threadSafe;
		Spinlock mapLock;
		StackSTLHashMap<T> map;
		RLockFreeTree<T> tree;
};

/**********************************************************/
template <class T>
AbstractStackTree<T>::AbstractStackTree(bool useHashMap, bool threadSafe)
	:tree(threadSafe)
{
	this->useHashMap = useHashMap;
	this->threadSafe = threadSafe;
}

/**********************************************************/
StackNodeHanlder AbstractStackTree::getChild(StackNodeHanlder handler, void* callsite)
{
	assert(useHashMap == false);
	assert(handler != NULL);
	if (useHashMap)
		return NULL;
	else
		return tree.getChild(handler,callsite);
}

/**********************************************************/
T* AbstractStackTree::getData(StackNodeHanlder handler)
{
	assert(handler != NULL);
	if (useHashMap)
	{
		return &(((StackSTLHashMap<T>::Node*)handler)->second);
	} else {
		return tree.getData((typename RLockFreeTreeNode<T>::Handler)handler);
	}
}

/**********************************************************/
StackNodeHanlder AbstractStackTree::getNode(const Stack& stack)
{
	StackNodeHanlder res;
	if (useHashMap)
	{
		MALT_OPTIONAL_CRITICAL(mapLock,threadSafe)
			res = map.getNode(stack);
		MALT_END_CRITICAL
	} else {
		res = tree.getFromStack(stack);
	}
	
	return res;
}

/**********************************************************/
StackNodeHanlder AbstractStackTree::getParent(StackNodeHanlder handler)
{
	assert(handler != NULL);
	assert(useHashMap == false);
	
	if (useHashMap)
		return NULL;
	else
		return tree.getParent(handler);
}

/**********************************************************/
void AbstractStackTree::toJson(htopml::JsonState& state)
{
	if (useHashMap)
		convertToJson(state,map);
	else
		convertToJson(state,tree);
}

}

#endif //MALT_RLOCK_FREE_TREE_HPP

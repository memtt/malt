/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_RLOCK_FREE_TREE_HPP
#define MALT_RLOCK_FREE_TREE_HPP

/********************  HEADERS  *********************/
//std
#include <cassert>
#include "StackSTLHashMap.hpp"
#include "RLockFreeTree.hpp"


/*******************  NAMESPACE  ********************/
namespace MALT
{
	
/*********************  TYPES  **********************/
typedef void * StackNodeHanlder;

/*********************  CLASS  **********************/
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

/*******************  FUNCTION  *********************/
template <class T>
AbstractStackTree<T>::AbstractStackTree(bool useHashMap, bool threadSafe)
	:tree(threadSafe)
{
	this->useHashMap = useHashMap;
	this->threadSafe = threadSafe;
}

/*******************  FUNCTION  *********************/
StackNodeHanlder AbstractStackTree::getChild(StackNodeHanlder handler, void* callsite)
{
	assert(useHashMap == false);
	assert(handler != NULL);
	if (useHashMap)
		return NULL;
	else
		return tree.getChild(handler,callsite);
}

/*******************  FUNCTION  *********************/
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

/*******************  FUNCTION  *********************/
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

/*******************  FUNCTION  *********************/
StackNodeHanlder AbstractStackTree::getParent(StackNodeHanlder handler)
{
	assert(handler != NULL);
	assert(useHashMap == false);
	
	if (useHashMap)
		return NULL;
	else
		return tree.getParent(handler);
}

/*******************  FUNCTION  *********************/
void AbstractStackTree::toJson(htopml::JsonState& state)
{
	if (useHashMap)
		convertToJson(state,map);
	else
		convertToJson(state,tree);
}

}

#endif //MALT_RLOCK_FREE_TREE_HPP

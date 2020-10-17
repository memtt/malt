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
//internal common
#include <common/SimpleAllocator.hpp>
//internal stacks
#include <stacks/EnterExitStack.hpp>
#include <core/SymbolSolver.hpp>
#include <json/JsonState.h>

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*********************  CLASS  **********************/
/**
 * Define the node for RLockFreeTree.
**/
struct RLockFreeTreeNode
{
	RLockFreeTreeNode(void*callSite);
	/** Pointer to the next element on the same parent. NULL if none (last element). **/
	RLockFreeTreeNode * next;
	/** Pointer to the parent node, NULL if root. **/
	RLockFreeTreeNode * parent;
	/** Pointer to the first child of the current node. NULL if none. **/
	RLockFreeTreeNode * firstChild;
	/** Define the corresponding call site. **/
	void * callSite;
	/** Optional data attached to the current node, NULL if none. **/
	void * data;
};

/*********************  CLASS  **********************/
/**
 * This class provide a basic tree based on nodes with childs and optional data attached to the nodes.
 * The child list is built with a simple linked list. As we do not consider node deletion we can access to all lists
 * without locks. We use locks only to insert new keys after checking their absence.
 * 
 * This tree might be used for the enter-exit mode in place of StackSTLHashMap.
 * 
 * @brief Short tree storage with lock free read access, no deletion and locked insertion. To be used for enter-exit mode.
**/
template <class T>
class RLockFreeTree
{
	public:
		typedef RLockFreeTreeNode * Handler;
	public:
		RLockFreeTree(bool threadSafe = true);
		Handler getRoot(void);
		Handler getChild(Handler handler,void * callsite);
		Handler getParent(Handler handler);
		Handler getFromStack(Stack & stack);
		T * getData(Handler handler);
		T * getDataFromStack(Stack & stack);
	public:
		template <class U> friend void convertToJson(htopml::JsonState & json, const RLockFreeTree<U> & value);
	private:
		RLockFreeTreeNode * findChild(RLockFreeTreeNode * node,void * callsite);
		RLockFreeTreeNode * addChild(RLockFreeTreeNode * node,void * callsite);
		void insertChild(RLockFreeTreeNode * parent,RLockFreeTreeNode * child);
	private:
		RLockFreeTreeNode root;
		Spinlock lock;
		bool threadSafe;
};

/*******************  FUNCTION  *********************/
inline RLockFreeTreeNode::RLockFreeTreeNode(void* callSite)
{
	this->data = NULL;
	this->callSite = callSite;
	this->next = NULL;
	this->parent = NULL;
	this->firstChild = NULL;
}

/*******************  FUNCTION  *********************/
template <class T>
RLockFreeTree<T>::RLockFreeTree(bool threadSafe)
	:root(NULL)
{
	this->threadSafe = threadSafe;
}

/*******************  FUNCTION  *********************/
template <class T>
typename RLockFreeTree<T>::Handler RLockFreeTree<T>::addChild(RLockFreeTreeNode* node, void* callsite)
{
	RLockFreeTreeNode * child = NULL;
	assert(node != NULL);

	MALT_OPTIONAL_CRITICAL(lock,threadSafe)
		//re-check if another thread hasn't insert it at same time
		child = findChild(node,callsite);
		
		//if already return it (we can safetly use return here between lock due to RAII support of MALT_OPTIONAL_CRITICAL)
		if (child != NULL)
			return child;
		
		//create a new one
		child = new RLockFreeTreeNode(callsite);
		child->parent = node;
		
		//insert
		insertChild(node,child);
	MALT_END_CRITICAL
	
	return child;
}

/*******************  FUNCTION  *********************/
/**
 * This function is lock free if we consider the locking of addChild and only insertion without deletion during use.
**/
template <class T>
RLockFreeTreeNode* RLockFreeTree<T>::findChild(RLockFreeTreeNode* node, void* callsite)
{
	//errors
	assert(node != NULL);
	
	//start on first child
	RLockFreeTreeNode * child = node->firstChild;
	RLockFreeTreeNode * res = NULL;
	
	//loop to next and check callsite
	while(child != NULL && res == NULL)
	{
		if (child->callSite == callsite)
			res = child;
		child = child->next;
	}
	
	return res;
}

/*******************  FUNCTION  *********************/
template <class T>
void RLockFreeTree<T>::insertChild(RLockFreeTreeNode* parent, RLockFreeTreeNode* child)
{
	//errors
	assert(parent != NULL);
	assert(child != NULL);
	
	//inert as first
	child->next = parent->firstChild;
	parent->firstChild = child;
}

/*******************  FUNCTION  *********************/
template <class T>
typename RLockFreeTree<T>::Handler RLockFreeTree<T>::getRoot(void)
{
	return &root;
}

/*******************  FUNCTION  *********************/
template <class T>
typename RLockFreeTree<T>::Handler RLockFreeTree<T>::getParent(RLockFreeTree::Handler handler)
{
	return handler->parent;
}

/*******************  FUNCTION  *********************/
template <class T>
typename RLockFreeTree<T>::Handler RLockFreeTree<T>::getChild(RLockFreeTree::Handler handler, void* callsite)
{
	//serch if exist
	RLockFreeTreeNode * node = findChild(handler,callsite);
	
	//if not found create the entry, it will lock
	if (node == NULL)
		node = addChild(handler,callsite);
	
	return node;
}

/*******************  FUNCTION  *********************/
template <class T>
T* RLockFreeTree<T>::getData(typename RLockFreeTree<T>::Handler handler)
{
	if (handler == NULL)
		return NULL;
	
	//read data addr
	T * data = (T*)handler->data;

	//if not exist, create
	if (data == NULL)
	{
		MALT_OPTIONAL_CRITICAL(lock,threadSafe)
			//maybe another do the job juste before, so recheck before create
			if (handler->data == NULL)
				handler->data = new (gblInternaAlloc->malloc(sizeof(T))) T ();
			data = (T*)handler->data;
		MALT_END_CRITICAL
	}
	
	return (T*)data;
}

/*******************  FUNCTION  *********************/
template <class U> 
void convertToJson(htopml::JsonState & json, const RLockFreeTreeNode * value,int & count)
{
	const RLockFreeTreeNode * node;
	
	//print if data
	if (value->data != NULL)
	{
		json.printListSeparator();
		json.openStruct();
		json.openFieldArray("stack");
		node = value;
		while (node != NULL)
		{
			if (node->callSite != NULL)
			{
				json.printValue(node->callSite);
			}
			node = node->parent;
		}
		json.closeFieldArray("stack");
		json.printField("infos",*(U*)value->data);
		json.closeStruct();
		count++;
	}
	
	//loop on childs
	node = value->firstChild;
	while (node != NULL)
	{
		convertToJson<U>(json,node,count);
		node = node->next;
	}
}

/*******************  FUNCTION  *********************/
template <class U> 
void convertToJson(htopml::JsonState & json, const RLockFreeTree<U> & value)
{
	int count = 0;

	json.openStruct();
	json.openFieldArray("stats");
	convertToJson<U>(json,&value.root,count);
	json.closeFieldArray("stats");
	json.printField("count",count);
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
template <class T>
T* RLockFreeTree<T>::getDataFromStack(Stack& stack)
{
	return getData(getFromStack(stack));
}

/*******************  FUNCTION  *********************/
template <class T>
typename RLockFreeTree<T>::Handler RLockFreeTree<T>::getFromStack(Stack& stack)
{
	typename RLockFreeTree<T>::Handler handler = getRoot();
	int size = stack.getSize();
	for (int i = 0 ; i < size ; i++)
		handler = getChild(handler,stack[i]);
	
	return handler;
}

}

#endif //MALT_RLOCK_FREE_TREE_HPP

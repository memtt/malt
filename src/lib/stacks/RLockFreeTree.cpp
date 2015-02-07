/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include "RLockFreeTree.hpp"
#include <json/JsonState.h>

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*******************  FUNCTION  *********************/
RLockFreeTreeNode::RLockFreeTreeNode(void * callsite)
{
	this->callSite = callsite;
	this->next = NULL;
	this->firstChild = NULL;
	this->parent = NULL;
	this->dataId = -1;
	this->data = NULL;
}

/*******************  FUNCTION  *********************/
RLockFreeTree::RLockFreeTree(bool threadSafe)
	:root(NULL)
{
	this->threadSafe = threadSafe;
	this->lastDataId = 0;
}

/*******************  FUNCTION  *********************/
RLockFreeTree::~RLockFreeTree(void)
{

}

/*******************  FUNCTION  *********************/
StackTreeHandler RLockFreeTree::enterThread(void)
{
	return &root;
}

/*******************  FUNCTION  *********************/
void RLockFreeTree::exitThread(StackTreeHandler handler)
{
}

/*******************  FUNCTION  *********************/
RLockFreeTreeNode * RLockFreeTree::addChild(RLockFreeTreeNode* node, void* callsite)
{
	RLockFreeTreeNode * child = NULL;
	assert(node != NULL);

	MATT_OPTIONAL_CRITICAL(lock,threadSafe)
		//re-check if another thread hasn't insert it at same time
		child = findChild(node,callsite);
		
		//if already return it (we can safetly use return here between lock due to RAII support of MATT_OPTIONAL_CRITICAL)
		if (child != NULL)
			return child;
		
		//create a new one
		child = new RLockFreeTreeNode(callsite);
		child->parent = node;
		child->dataId = lastDataId++;
		
		//insert
		insertChild(node,child);
	MATT_END_CRITICAL
	
	return child;
}

/*******************  FUNCTION  *********************/
RLockFreeTreeNode * RLockFreeTree::findChild(MATT::RLockFreeTreeNode* node, void* callsite)
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
void RLockFreeTree::insertChild(RLockFreeTreeNode* parent, RLockFreeTreeNode* child)
{
	//errors
	assert(parent != NULL);
	assert(child != NULL);
	
	//inert as first
	child->next = parent->firstChild;
	parent->firstChild = child;
}

/*******************  FUNCTION  *********************/
StackTreeHandler RLockFreeTree::setOnRoot(StackTreeHandler handler)
{
	return &this->root;
}

/*******************  FUNCTION  *********************/
StackTreeHandler RLockFreeTree::setFromStack(StackTreeHandler handler, const Stack & stack)
{
	//check
	assert(stack.isValid());
	
	int size = stack.getSize();
	setOnRoot(handler);
	for (int i = 0 ; i < size ; i++)
		handler = enterFunction(handler,stack[i]);
	
	return handler;
}

/*******************  FUNCTION  *********************/
RLockFreeTreeNode* RLockFreeTree::getNode(StackTreeHandler handler)
{
	return (RLockFreeTreeNode*)handler;
}

/*******************  FUNCTION  *********************/
StackTreeHandler RLockFreeTree::enterFunction(StackTreeHandler handler, void* callsite)
{
	Handler node = findChild((Handler)handler,callsite);
	
	//if not found create the entry, it will lock
	if (node == NULL)
		node = addChild((Handler)handler,callsite);
	
	return node;
}

/*******************  FUNCTION  *********************/
StackTreeHandler RLockFreeTree::exitFunction(StackTreeHandler handler, void* callsite)
{
	Handler typedHandler = (Handler)handler;
	typedHandler = typedHandler->parent;
	return handler;
}

/*******************  FUNCTION  *********************/
void* RLockFreeTree::getData(StackTreeHandler handler)
{
// 	assert(id < MATT_STACK_MAX_DATA);
	Handler typedHandler = (Handler)handler;
	return typedHandler->data;
}

/*******************  FUNCTION  *********************/
void RLockFreeTree::setData(StackTreeHandler handler, void* data)
{
	Handler typedHandler = (Handler)handler;
	typedHandler->data = data;
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const RLockFreeTree& tree)
{
	convertToJson(json,tree.root);
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const RLockFreeTreeNode& tree)
{
	char buffer[64];
	json.openStruct();
	
		//data ID
		if (tree.data != NULL)
			json.printField("dataId",tree.dataId);
		
		if (tree.firstChild != NULL)
		{
			RLockFreeTreeNode * cur = tree.firstChild;
			while (cur != NULL)
			{
				sprintf(buffer,"%p",cur->callSite);
				json.printField(buffer,*cur);
				cur = cur->next;
			}
		}
	json.closeStruct();
}

}

/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include "RLockFreeTree.hpp"
#include <json/JsonState.h>
#include <common/Debug.hpp>

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
	this->hasChildData = false;
	for (int i = 0 ; i < MATT_STACK_TREE_ENTRIES ; i++)
		this->data[i] = NULL;
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
StackTreeHandler RLockFreeTree::getFromStack(StackTreeHandler handler, const Stack & stack)
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
void RLockFreeTree::copyData(const MATT::Stack& stack, const MATT::StackTreeStorage& storage,int id)
{
	StackTreeHandler handler = enterThread();
	handler = getFromStack(handler,stack);
	MATT::RLockFreeTree::Handler typedHandler = getNode(handler);
	typedHandler->data = storage;
// 	typedHandler->dataId = id;
	exitThread(handler);
}

/*******************  FUNCTION  *********************/
StackTreeHandler RLockFreeTree::getFromStack(StackTreeHandler handler, int skip)
{
	BacktraceStack stack;
	stack.loadCurrentStack();
	stack.fastSkip(skip+1);
	return getFromStack(handler,stack);
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
StackTreeDataHandler RLockFreeTree::getDataHandler ( StackTreeHandler handler )
{
	Handler typedHandler = (Handler)handler;
	return &typedHandler->data;
}

/*******************  FUNCTION  *********************/
int RLockFreeTree::getStackId ( StackTreeDataHandler handler )
{
	Handler typedHandler = (Handler)handler;
	assert(typedHandler->dataId != -1);
	return typedHandler->dataId;
}

/*******************  FUNCTION  *********************/
void* RLockFreeTree::getData(MATT::StackTreeHandler handler, int id)
{
	assert(id < MATT_STACK_TREE_ENTRIES);
	StackTreeDataHandler dataHandler = getDataHandler(handler);
	void * ret = (*dataHandler)[id];
	if (ret == NULL)
		ret = (*dataHandler)[id] = descriptors[id]->allocate();
	return ret;
}

/*******************  FUNCTION  *********************/
void* RLockFreeTree::getData ( StackTreeDataHandler dataHandler, int id )
{
	void * ret = (*dataHandler)[id];
	if (ret == NULL)
		ret = (*dataHandler)[id] = descriptors[id]->allocate();
	return ret;
}

/*******************  FUNCTION  *********************/
// void RLockFreeTree::setData(MATT::StackTreeHandler handler, int id, void* data)
// {
// 	assert(id < MATT_STACK_TREE_ENTRIES);
// 	Handler typedHandler = (Handler)handler;
// 	typedHandler->data[id] = data;
// }

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const RLockFreeTree& tree)
{
	json.openStruct();
		json.printField("calltree",tree.root);
		json.openFieldStruct("data");
		
		for (int i = 0 ; i < MATT_STACK_TREE_ENTRIES ; i++)
			if (tree.descriptors[i] != NULL)
				tree.printData(json,i);
		json.closeFieldStruct("data");
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const RLockFreeTreeNode& tree)
{
	char buffer[64];
	json.openStruct();
		//data ID
		bool haveData = false;
		for (int i = 0 ; i < MATT_STACK_TREE_ENTRIES ; i++)
			if (tree.data[i] != NULL)
				haveData = true;
		
		if (tree.dataId >= 0 && haveData)
			json.printField("dataId",tree.dataId);
		
		if (tree.firstChild != NULL && tree.hasChildData)
		{
			RLockFreeTreeNode * cur = tree.firstChild;
			while (cur != NULL)
			{
				sprintf(buffer,"%p",cur->callSite);
				if (cur->hasChildData)
					json.printField(buffer,*cur);
				cur = cur->next;
			}
		}
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
void RLockFreeTree::printData(htopml::JsonState& json, int i) const
{
	json.openFieldStruct(this->names[i].c_str());
		this->printData(json,&root,i);
	json.closeFieldStruct(this->names[i].c_str());
}

/*******************  FUNCTION  *********************/
void RLockFreeTree::printData(htopml::JsonState& json, const RLockFreeTreeNode* node, int i) const
{
	char buffer[64];
	if (node->data[i] != NULL && node->dataId >= 0)
	{
		sprintf(buffer,"%d",node->dataId);
		descriptors[i]->toJson(json,buffer,node->data[i]);
	}
	RLockFreeTreeNode * cur = node->firstChild;
	while (cur != NULL)
	{
		printData(json,cur,i);
		cur = cur->next;
	}
}

/*******************  FUNCTION  *********************/
bool RLockFreeTree::isEnterExit ( void ) const
{
	return true;
}

/*******************  FUNCTION  *********************/
void RLockFreeTree::toJson ( htopml::JsonState& json, const MATT::StackTree& tree ) const
{
	convertToJson(json,dynamic_cast<const RLockFreeTree&>(tree));
}

/*******************  FUNCTION  *********************/
bool RLockFreeTreeNode::hasData ( void )
{
	for (int i = 0 ; i < MATT_STACK_TREE_ENTRIES ; i++)
	{
		if (data[i] != NULL)
		{
			return true;
		}
	}
	
	if (dataId != -1)
		return true;
	else
		return false;
}

/*******************  FUNCTION  *********************/
void RLockFreeTree::markChildData ( MATT::RLockFreeTreeNode* node )
{
	//set root
	if (node == NULL)
		node = &this->root;
	
	//check current
	if (node->hasData())
	{
		RLockFreeTreeNode * cur = node;
		while (cur != NULL)
		{
			cur->hasChildData = true;
			cur = cur->parent;
		}
	}
	
	//childs
	RLockFreeTreeNode * cur = node->firstChild;
	while(cur != NULL)
	{
		markChildData(cur);
		cur = cur->next;
	}
}

/*******************  FUNCTION  *********************/
void RLockFreeTree::prepareForOutput ( void )
{
	markChildData();
}

}

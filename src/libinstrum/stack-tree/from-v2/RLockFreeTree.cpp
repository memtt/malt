/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/stack-tree/from-v2/RLockFreeTree.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2015
*    AUTHOR   : Bastien Levasseur - 2024
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include "RLockFreeTree.hpp"
#include <json/JsonState.h>
#include <common/Debug.hpp>
#include <common/NoFreeAllocator.hpp>
// #include <core/SymbolRegistry.hpp>

/**********************************************************/
namespace MALTV2
{

/**********************************************************/
RLockFreeTreeNode::RLockFreeTreeNode(MALT::LangAddress callsite)
{
	this->callSite = callsite;
	this->next = NULL;
	this->firstChild = NULL;
	this->parent = NULL;
	this->dataId = -1;
	this->hasChildData = false;
	for (int i = 0 ; i < MALT_STACK_TREE_ENTRIES ; i++)
		this->data[i] = NULL;
}

/**********************************************************/
RLockFreeTree::RLockFreeTree(bool threadSafe)
	:root(MALT::nullAddr)
{
	this->threadSafe = threadSafe;
	this->lastDataId = 0;
}

/**********************************************************/
RLockFreeTree::~RLockFreeTree(void)
{

}

/**********************************************************/
StackTreeHandler RLockFreeTree::enterThread(void)
{
	return &root;
}

/**********************************************************/
void RLockFreeTree::exitThread(StackTreeHandler handler)
{
}

/**********************************************************/
RLockFreeTreeNode * RLockFreeTree::addChild(RLockFreeTreeNode* node, MALT::LangAddress callsite)
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
		child = MALT_NO_FREE_NEW(RLockFreeTreeNode)(callsite);
		child->parent = node;
		child->dataId = lastDataId++;
		
		//insert
		insertChild(node,child);
	MALT_END_CRITICAL
	
	return child;
}

/**********************************************************/
RLockFreeTreeNode * RLockFreeTree::findChild(RLockFreeTreeNode* node, MALT::LangAddress callsite)
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

/**********************************************************/
void RLockFreeTree::insertChild(RLockFreeTreeNode* parent, RLockFreeTreeNode* child)
{
	//errors
	assert(parent != NULL);
	assert(child != NULL);
	
	//inert as first
	child->next = parent->firstChild;
	parent->firstChild = child;
}

/**********************************************************/
StackTreeHandler RLockFreeTree::setOnRoot(StackTreeHandler handler)
{
	return &this->root;
}

/**********************************************************/
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

/**********************************************************/
StackTreeHandler RLockFreeTree::getFromStack(StackTreeHandler handler, const MALT::Stack & stack)
{
	//check
	assert(stack.isValid());
	
	int size = stack.getSize();
	handler = setOnRoot(handler);
	for (int i = 0 ; i < size ; i++)
		handler = enterFunction(handler,stack[i]);
	
	return handler;
}

/**********************************************************/
void RLockFreeTree::copyData(const Stack& stack, const StackTreeStorage& storage,int id)
{
	StackTreeHandler handler = enterThread();
	handler = getFromStack(handler,stack);
	RLockFreeTree::Handler typedHandler = getNode(handler);
	typedHandler->data = storage;
// 	typedHandler->dataId = id;
	exitThread(handler);
}

/**********************************************************/
StackTreeHandler RLockFreeTree::getFromStack(StackTreeHandler handler, int skip)
{
	BacktraceStack stack;
	stack.loadCurrentStack();
	stack.fastSkip(skip+1);
	return getFromStack(handler,stack);
}

/**********************************************************/
RLockFreeTreeNode* RLockFreeTree::getNode(StackTreeHandler handler)
{
	return (RLockFreeTreeNode*)handler;
}

/**********************************************************/
StackTreeHandler RLockFreeTree::enterFunction(StackTreeHandler handler, MALT::LangAddress callsite)
{
	Handler node = findChild((Handler)handler,callsite);
	
	//if not found create the entry, it will lock
	if (node == NULL)
		node = addChild((Handler)handler,callsite);
	
	return node;
}

/**********************************************************/
StackTreeHandler RLockFreeTree::exitFunction(StackTreeHandler handler, MALT::LangAddress callsite)
{
	Handler typedHandler = (Handler)handler;
	typedHandler = typedHandler->parent;
	return handler;
}

/**********************************************************/
StackTreeDataHandler RLockFreeTree::getDataHandler ( StackTreeHandler handler )
{
	Handler typedHandler = (Handler)handler;
	return &typedHandler->data;
}

/**********************************************************/
int RLockFreeTree::getStackId ( StackTreeDataHandler handler )
{
	Handler typedHandler = (Handler)handler;
	assert(typedHandler->dataId != -1);
	return typedHandler->dataId;
}

/**********************************************************/
void* RLockFreeTree::getData(StackTreeHandler handler, int id)
{
	assert(id < MALT_STACK_TREE_ENTRIES);
	StackTreeDataHandler dataHandler = getDataHandler(handler);
	void * ret = (*dataHandler)[id];
	if (ret == NULL)
		ret = (*dataHandler)[id] = descriptors[id]->allocate();
	return ret;
}

/**********************************************************/
void* RLockFreeTree::getData ( StackTreeDataHandler dataHandler, int id )
{
	assert(id < MALT_STACK_TREE_ENTRIES);
	void * ret = (*dataHandler)[id];
	if (ret == NULL)
		ret = (*dataHandler)[id] = descriptors[id]->allocate();
	return ret;
}

/**********************************************************/
// void RLockFreeTree::setData(MALT::StackTreeHandler handler, int id, void* data)
// {
// 	assert(id < MALT_STACK_TREE_ENTRIES);
// 	Handler typedHandler = (Handler)handler;
// 	typedHandler->data[id] = data;
// }

/**********************************************************/
void convertToJson(htopml::JsonState& json, const std::map<void*,void*>  & value)
{
	char buffer[128];
	json.openStruct();
		for (std::map<void*,void*>::const_iterator it = value.begin() ; it != value.end() ; ++it)
		{
			sprintf(buffer,"%p",it->first);
			json.printField(buffer,it->second);
		}
	json.closeStruct();
}

/**********************************************************/
void convertToJson(htopml::JsonState& json, const RLockFreeTree& tree)
{
	char buffer[128];
	json.openStruct();
		json.openFieldStruct("addresses");
			for (std::map<MALT::LangAddress,MALT::LangAddress>::const_iterator it = tree.addrToId.begin() ; it != tree.addrToId.end() ; ++it)
			{
				if (it->first != MALT::nullAddr)
				{
					json.printField(it->second.toString().c_str(), it->first);
				}
			}
		json.closeFieldStruct("addresses");
		
		json.printField("calltree",tree.root);
		json.openFieldStruct("data");
		
		for (int i = 0 ; i < MALT_STACK_TREE_ENTRIES ; i++)
			if (tree.descriptors[i] != NULL)
				tree.printData(json,i);
		json.closeFieldStruct("data");
	json.closeStruct();
}

/**********************************************************/
void convertToJson(htopml::JsonState& json, const RLockFreeTreeNode& tree)
{
	char buffer[64];
	json.openStruct();
		//data ID
		bool haveData = false;
		for (int i = 0 ; i < MALT_STACK_TREE_ENTRIES ; i++)
			if (tree.data[i] != NULL)
				haveData = true;
		
		if (tree.dataId >= 0 && haveData)
			json.printField("dataId",tree.dataId);
		
		if (tree.firstChild != NULL && tree.hasChildData)
		{
			RLockFreeTreeNode * cur = tree.firstChild;
			while (cur != NULL)
			{
				if (cur->hasChildData)
					json.printField(cur->callSite.toString().c_str(),*cur);
				cur = cur->next;
			}
		}
	json.closeStruct();
}

/**********************************************************/
void RLockFreeTree::printData(htopml::JsonState& json, int i) const
{
	json.openFieldStruct(this->names[i].c_str());
		this->printData(json,&root,i);
	json.closeFieldStruct(this->names[i].c_str());
}

/**********************************************************/
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

/**********************************************************/
bool RLockFreeTree::isEnterExit ( void ) const
{
	return true;
}

/**********************************************************/
void RLockFreeTree::toJson ( htopml::JsonState& json, const StackTree& tree ) const
{
	convertToJson(json,dynamic_cast<const RLockFreeTree&>(tree));
}

/**********************************************************/
bool RLockFreeTreeNode::hasData ( void )
{
	for (int i = 0 ; i < MALT_STACK_TREE_ENTRIES ; i++)
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

/**********************************************************/
void RLockFreeTree::markChildData ( RLockFreeTreeNode* node )
{
	//set root
	if (node == NULL)
		node = &this->root;
	
	//update addree
	std::map<MALT::LangAddress,MALT::LangAddress>::iterator it = addrToId.find(node->callSite);
	//TODO: Create a third domain ID in LangAddress
	MALT::LangAddress id(MALT::DOMAIN_C, (void*)addrToId.size());
	if (it != addrToId.end())
		id = addrToId[node->callSite];
	else
		addrToId[node->callSite] = id;
	node->callSite = id;
	
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

/**********************************************************/
void RLockFreeTree::prepareForOutput ( void )
{
	addrToId[MALT::nullAddr] = MALT::nullAddr;
	markChildData();
}

/**********************************************************/
void RLockFreeTree::registerSymbols ( SymbolRegistry& solver ) const
{
	root.registerSymbols(solver);
}

/**********************************************************/
void RLockFreeTreeNode::registerSymbols ( SymbolRegistry& solver ) const
{
	MALT_FATAL("Must not be called in V1 (not supported)");
// 	solver.registerAddress(callSite);
// 	RLockFreeTreeNode * cur = firstChild;
// 	while (cur != NULL)
// 	{
// 		solver.registerAddress(cur->callSite);
// 		cur->registerSymbols(solver);
// 	}
}

}

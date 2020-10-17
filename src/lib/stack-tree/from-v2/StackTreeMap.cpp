/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <cassert>
#include "StackTreeMap.hpp"
#include "BacktraceStack.hpp"
#include "EnterExitStack.hpp"
#include "RLockFreeTree.hpp"
#include <common/Debug.hpp>
#include <common/Helpers.hpp>
#include <common/SimpleAllocator.hpp>
// #include <core/SymbolRegistry.hpp>

/*******************  NAMESPACE  ********************/
namespace MALTV2
{

/*******************  FUNCTION  *********************/
StackTreeMap::Key::Key(const Stack* stack,int dataId)
{
	//check
	assert(stack != NULL);
	assert(dataId >= 0);
	
	//setup data
	this->stack = stack;
	this->hash = stack->hash();
	this->dataId =  dataId;
}

/*******************  FUNCTION  *********************/
bool StackTreeMap::Key::operator==(const Key& node) const
{
	if (!(hash == node.hash))
		return false;
	else
		return *stack == *node.stack;
}

/*******************  FUNCTION  *********************/
bool StackTreeMap::Key::operator<(const Key& node) const
{
	if (hash < node.hash)
	{
		return true;
	} else if (hash == node.hash) {
		return *stack < *node.stack;
	} else {
		return false;
	}
}

/*******************  FUNCTION  *********************/
void StackTreeMap::Key::cloneStack(void)
{
	assert(stack != NULL);
	void * ptr = MALT_MALLOC(sizeof(Stack));
	stack = new(ptr) Stack(*stack);
}

/*******************  FUNCTION  *********************/
StackTreeMap::StackTreeMap(bool backtrace,bool threadsafe)
	: StackTree()
{
	this->backtrace = backtrace;
	this->threadSafe = threadsafe;
	this->nextId = 0;
}

/*******************  FUNCTION  *********************/
StackTreeMap::~StackTreeMap(void)
{

}

/*******************  FUNCTION  *********************/
StackTreeHandler StackTreeMap::enterThread(void)
{
	Handler * handler = MALT_NO_FREE_NEW(Handler);
	handler->storage = NULL;
	return handler;
}

/*******************  FUNCTION  *********************/
StackTreeHandler StackTreeMap::exitFunction(StackTreeHandler handler, void* callsite)
{
	//assume(!backtrace,"Try to use exitFunction with backtrace mode enabled !");
	if (backtrace)
	{
		Handler * typedHandler = (Handler*)handler;
		typedHandler->enterExitStack.exitFunction(callsite);
		typedHandler->storage = NULL;
		return handler;
	} else {
		return handler;
	}
}

/*******************  FUNCTION  *********************/
StackTreeHandler StackTreeMap::enterFunction(StackTreeHandler handler, void* callsite)
{
	//assume(!backtrace,"Try to use exitFunction with backtrace mode enabled !");
	if (backtrace)
	{
		Handler * typedHandler = (Handler*)handler;
		typedHandler->enterExitStack.enterFunction(callsite);
		typedHandler->storage = NULL;
		return handler;
	} else {
		return handler;
	}
}

/*******************  FUNCTION  *********************/
void StackTreeMap::exitThread(StackTreeHandler handler)
{
// 	Handler * typedHandler = (Handler*)handler;
	//delete typedHandler;
}

/*******************  FUNCTION  *********************/
int StackTreeMap::getStackId ( StackTreeDataHandler handler )
{
	Handler * typedHandler = (Handler*)handler;
	return typedHandler->stackId;
}

/*******************  FUNCTION  *********************/
StackTreeDataHandler StackTreeMap::getDataHandler ( StackTreeHandler handler )
{
	//get handler
	Handler * typedHandler = (Handler*)handler;
	StackTreeStorage * storage = typedHandler->storage;
	
	//check if ok
	if (storage == NULL)
	{
		//get stack
		if (backtrace)
			typedHandler = (Handler*)getFromStack(handler,typedHandler->backtraceStack);
		else
			typedHandler = (Handler*)getFromStack(handler,typedHandler->enterExitStack);
		storage = typedHandler->storage;
	}
	
	return storage;
}

/*******************  FUNCTION  *********************/
void* & StackTreeMap::internalGetData(StackTreeHandler handler, int id)
{
	assert(id < MALT_STACK_TREE_ENTRIES);

	//get handler
	StackTreeStorage * storage = getDataHandler(handler);
	
	//entry
	void * & entry = (*storage)[id];
	
	//allocate
	if (entry == NULL)
		entry = descriptors[id]->allocate();
	
	//return
	return entry;
}

/*******************  FUNCTION  *********************/
void* StackTreeMap::getData(StackTreeHandler handler, int id)
{
	return internalGetData(handler,id);
}

/*******************  FUNCTION  *********************/
void* StackTreeMap::getData ( StackTreeDataHandler handler, int id )
{
	//entry
	void * entry = (*handler)[id];
	
	//allocate
	if (entry == NULL)
		(*handler)[id] = descriptors[id]->allocate();
	
	//return
	return (*handler)[id];
}

/*******************  FUNCTION  *********************/
StackTreeHandler StackTreeMap::getFromStack(StackTreeHandler handler, const Stack& stack)
{
	//get handler
	Handler * typedHandler = (Handler*)handler;
	NodeMap::iterator it;
	
	//build key
	Key key(&stack,nextId);
	
	MALT_OPTIONAL_CRITICAL(lock,threadSafe)
	
		//search
		it = map.find(key);
		
		//if not found, create
		if (it == map.end())
		{
			key.cloneStack();
			nextId++;
			StackTreeStorage storage;
			map[key] = storage;
			it = map.find(key);
		}
	
	MALT_END_CRITICAL
	
	//return
	typedHandler->stackId = it->first.dataId;
	typedHandler->storage = &(it->second);
	return handler;
}

/*******************  FUNCTION  *********************/
StackTreeHandler StackTreeMap::getFromStack(StackTreeHandler handler, int skip)
{
	assume(backtrace,"Invlid mode, require backtrace enabled !");
	//get handler
	Handler * typedHandler = (Handler*)handler;
	typedHandler->backtraceStack.loadCurrentStack();
	typedHandler->backtraceStack.fastSkip(skip+1);
	return getFromStack(handler,typedHandler->backtraceStack);
}

/*******************  FUNCTION  *********************/
void StackTreeMap::prepareForOutput ( void )
{

}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const StackTreeMap& tree)
{
	RLockFreeTree otree;
	StackTreeHandler handler = otree.enterThread();
	
	//copy descr
	for (int i = 0 ; i < MALT_STACK_TREE_ENTRIES ; i++)
		if (tree.descriptors[i] != NULL)
			otree.addDescriptor(tree.names[i],tree.descriptors[i]);
		
	//copy values
	for(StackTreeMap::NodeMap::const_iterator it = tree.map.begin() ; it != tree.map.end() ; ++it)
		otree.copyData(*it->first.stack,it->second,it->first.dataId);
	
	otree.exitThread(handler);
	otree.markChildData();
	
	convertToJson(json,otree);
}

/*******************  FUNCTION  *********************/
bool StackTreeMap::isEnterExit ( void ) const
{
	return !backtrace;
}

/*******************  FUNCTION  *********************/
void StackTreeMap::toJson ( htopml::JsonState& json, const StackTree& tree ) const
{
	convertToJson(json,dynamic_cast<const StackTreeMap&>(tree));
}

/*******************  FUNCTION  *********************/
void StackTreeMap::registerSymbols ( SymbolRegistry& registry ) const
{
	MALT_FATAL("Not supported in V1");
// 	for(StackTreeMap::NodeMap::const_iterator it = map.begin() ; it != map.end() ; ++it)
// 		it->first.stack->registerSymbols(registry);
}

}

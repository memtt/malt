/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <cassert>
#include "StackTreeMap.hpp"
#include "BacktraceStack.hpp"
#include "EnterExitStack.hpp"
#include "RLockFreeTree.hpp"
#include "StackLoopRemover.hpp"
#include <common/Debug.hpp>
#include <common/Helpers.hpp>
#include <allocators/NoFreeAllocator.hpp>
#include <core/SymbolRegistry.hpp>

/*******************  NAMESPACE  ********************/
namespace MATT
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
	void * ptr = MATT_MALLOC(sizeof(Stack));
	stack = new(ptr) Stack(*stack);
}

/*******************  FUNCTION  *********************/
StackTreeMap::StackTreeMap(bool backtrace,bool threadsafe)
	: StackTree()
{
	this->backtrace = backtrace;
	this->threadSafe = threadsafe;
	this->nextId = 0;
	this->loopSuppress = false;
}

/*******************  FUNCTION  *********************/
StackTreeMap::~StackTreeMap(void)
{

}

/*******************  FUNCTION  *********************/
StackTreeHandler StackTreeMap::enterThread(void)
{
	void * ptr = MATT_NO_FREE_MALLOC(sizeof(Handler));
	Handler * handler = new(ptr) Handler;
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
StackId StackTreeMap::getStackId ( MATT::StackTreeDataHandler handler )
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
	assert(id < MATT_STACK_TREE_ENTRIES);

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
	
	MATT_OPTIONAL_CRITICAL(lock,threadSafe)
	
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
	
	MATT_END_CRITICAL
	
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
// 	std::map< StackId, StackId > idmap;
	
	//copy descr
	for (int i = 0 ; i < MATT_STACK_TREE_ENTRIES ; i++)
		if (tree.descriptors[i] != NULL)
			otree.addDescriptor(tree.names[i],tree.descriptors[i]);
	
	if (tree.loopSuppress)
	{
		//search max size
		int maxSize = 0;
		foreachConst(StackTreeMap::NodeMap,tree.map,it)
		{
			const Stack * stack = it->first.stack;
			if (stack->getSize() > maxSize)
				maxSize = stack->getSize();
		}
	
		//apply loop loopSuppress
		StackLoopRemover remover(maxSize);
		Stack tmpStack(STACK_ORDER_ASC);
		foreachConst(StackTreeMap::NodeMap,tree.map,it)
		{
			const Stack * stack = it->first.stack;
			tmpStack = *stack;
			remover.removeLoops(tmpStack);
			otree.mergeData(tmpStack,it->second,it->first.dataId);
/*			
			StackTreeHandler ohandler;
			ohandler = otree.getFromStack(handler,tmpStack);
			StackTreeDataHandler oDataHandler = otree.getDataHandler(ohandler);
			StackId oid = otree.getStackId(oDataHandler);
			
			idmap[it->first.dataId] = oid;*/
		}
	} else {
		//copy values
		foreachConst(StackTreeMap::NodeMap,tree.map,it)
			otree.copyData(*it->first.stack,it->second,it->first.dataId);
	}
	
	otree.exitThread(handler);
	otree.prepareForOutput();
	
	convertToJson(json,otree);
	
// 	json.openFieldStruct("stackIds");
// 		for(std::map<StackId,StackId>::const_iterator it = idmap.begin() ; it != idmap.end() ; ++it)
// 		{
// 			char buffer[256];
// 			sprintf(buffer,"%lu",it->first);
// 			json.printField(buffer,it->second);
// 		}
// 	json.closeFieldStruct("stackIds");
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
	foreachConst(StackTreeMap::NodeMap,map,it)
		it->first.stack->registerSymbols(registry);
}

}
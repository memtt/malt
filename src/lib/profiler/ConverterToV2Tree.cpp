/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//standard
#include "ConverterToV2Tree.hpp"

/*******************  NAMESPACE  ********************/
namespace MALT
{

void convertToV2Tree ( MALTV2::RLockFreeTree& tree, StackSTLHashMap< CallStackInfo >& orig )
{
	//register entries
	int id = tree.addDescriptor<CallStackInfoGlobals>("globals");
	assumeArg(id == INFO_GLOBALS,"Do not get valid ID while registering descriptors : %1 != %2").arg(id).arg(INFO_GLOBALS).end();
	
	id = tree.addDescriptor<CallStackInfoRealloc>("realloc");
	assumeArg(id == INFO_REALLOC,"Do not get valid ID while registering descriptors : %1 != %2").arg(id).arg(INFO_REALLOC).end();
	
	id = tree.addDescriptor<SimpleQuantityHistoryPtr>("alloc");
	assumeArg(id == INFO_ALLOC,"Do not get valid ID while registering descriptors : %1 != %2").arg(id).arg(INFO_ALLOC).end();
	
	id = tree.addDescriptor<SimpleQuantityHistoryPtr>("free");
	assumeArg(id == INFO_FREE,"Do not get valid ID while registering descriptors : %1 != %2").arg(id).arg(INFO_FREE).end();
	
	id = tree.addDescriptor<SimpleQuantityHistoryPtr>("lifetime");
	assumeArg(id == INFO_LIFETIME,"Do not get valid ID while registering descriptors : %1 != %2").arg(id).arg(INFO_LIFETIME).end();
		
	//fill
	while (orig.empty() == false)
	{
		StackSTLHashMap<CallStackInfo>::iterator it = orig.begin();

		//get handler
		MALTV2::StackTreeHandler handler = MALT_STACK_TREE_NULL;
		handler = tree.getFromStack(handler,*(it->first.stack));
		MALTV2::StackTreeDataHandler dataHandler = tree.getDataHandler(handler);
		
		//copy globals
		CallStackInfoGlobals &globals = tree.getTypedData<CallStackInfoGlobals>(dataHandler,INFO_GLOBALS);
		convert(globals,it->second);
		
		//copy realloc if need
		if (it->second.hasRealloc())
		{
			CallStackInfoRealloc &reallocInfo = tree.getTypedData<CallStackInfoRealloc>(dataHandler,INFO_REALLOC);
			convert(reallocInfo,it->second);
		}
		
		//copy malloc info if need
		const SimpleQuantityHistory * tmp;
		tmp = it->second.getAllocInfo();
		if (tmp->count > 0)
			tree.getTypedData<SimpleQuantityHistoryPtr>(dataHandler,INFO_ALLOC).ptr = tmp;
		
		//copy free info if need
		tmp = it->second.getFreeInfo();
		if (tmp->count > 0)
			tree.getTypedData<SimpleQuantityHistoryPtr>(dataHandler,INFO_FREE).ptr = tmp;
		
		//copy lifetime info if need
		tmp = it->second.getLifetime();
		if (tmp->count > 0)
			tree.getTypedData<SimpleQuantityHistoryPtr>(dataHandler,INFO_LIFETIME).ptr = tmp;
		
		//remove it to free memory
		orig.remove(it);
	}
}

void convertToJson ( htopml::JsonState& json, const SimpleQuantityHistoryPtr& value )
{
	convertToJson(json,*value.ptr);
}

void convertToJson ( htopml::JsonState& json, const CallStackInfoRealloc& value )
{
	json.openStruct();
		json.printField("count",value.reallocCount);
		json.printField("delta",value.reallocDelta);
	json.closeStruct();
}

void convertToJson ( htopml::JsonState& json, const CallStackInfoGlobals& value )
{
	json.openStruct();
		json.printField("alive",value.alive);
		json.printField("zero",value.cntZeros);
		json.printField("alive",value.maxAlive);
		json.printField("peak",value.peak);
	json.closeStruct();
}

void convert ( CallStackInfoGlobals& out, const CallStackInfo& in )
{
	out.alive = in.alive;
	out.cntZeros = in.cntZeros;
	out.maxAlive = in.maxAlive;
	out.peak = in.peak;
}

void convert ( CallStackInfoRealloc& out, const CallStackInfo& in )
{
	out.reallocCount = in.reallocCount;
	out.reallocDelta = in.reallocDelta;
}

}

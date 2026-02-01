/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2022
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/profiler/ConverterToV2Tree.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2015 - 2022
***********************************************************/

/**********************************************************/
//standard
#include "ConverterToV2Tree.hpp"

/**********************************************************/
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
		const SimpleQuantityHistory & allocInfos = it->second.getAllocInfo();
		if (allocInfos.count > 0)
			tree.getTypedData<SimpleQuantityHistoryPtr>(dataHandler,INFO_ALLOC).ptr = &allocInfos;
		
		//copy free info if need
		const SimpleQuantityHistory & freeInfo = it->second.getFreeInfo();
		if (freeInfo.count > 0)
			tree.getTypedData<SimpleQuantityHistoryPtr>(dataHandler,INFO_FREE).ptr = &freeInfo;
		
		//copy lifetime info if need
		const SimpleQuantityHistory & lifeInfos = it->second.getLifetime();
		if (lifeInfos.count > 0)
			tree.getTypedData<SimpleQuantityHistoryPtr>(dataHandler,INFO_LIFETIME).ptr = &lifeInfos;
		
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
		json.printField("zero",value.cntZeros);
		json.printField("alive",value.alive[MEM_DOMAIN_CPU]);
		json.printField("maxAlive",value.maxAlive[MEM_DOMAIN_CPU]);
		json.printField("peak",value.peak[MEM_DOMAIN_CPU]);
		json.printField("aliveGPU",value.alive[MEM_DOMAIN_GPU]);
		json.printField("maxAliveGPU",value.maxAlive[MEM_DOMAIN_GPU]);
		json.printField("peakGPU",value.peak[MEM_DOMAIN_GPU]);
	json.closeStruct();
}

void convert ( CallStackInfoGlobals& out, const CallStackInfo& in )
{
	out.cntZeros = in.cntZeros;
	for (size_t i = 0 ; i < MEM_DOMAIN_COUNT ; i++) {
		out.alive[i] = in.alive[i];
		out.maxAlive[i] = in.maxAlive[i];
		out.peak[i] = in.peak[i];
	}
}

void convert ( CallStackInfoRealloc& out, const CallStackInfo& in )
{
	out.reallocCount = in.reallocCount;
	out.reallocDelta = in.reallocDelta;
}

}

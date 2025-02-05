/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/profiler/ConverterToV2Tree.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2015 - 2024
***********************************************************/

#ifndef MALT_CONVERT_TO_V2_TREE_HPP
#define MALT_CONVERT_TO_V2_TREE_HPP

/**********************************************************/
//standard
#include "./AllocStackProfiler.hpp"
#include "./../stack-tree/from-v2/RLockFreeTree.hpp"

/**********************************************************/
namespace MALT
{
	
enum CallStackInfoEnum
{
	INFO_GLOBALS,
	INFO_REALLOC,
	INFO_ALLOC,
	INFO_FREE,
	INFO_LIFETIME
};
	
struct CallStackInfoGlobals
{
	ssize_t cntZeros;
	ssize_t alive;
	ssize_t maxAlive;
	ssize_t peak;
};

struct CallStackInfoRealloc
{
	size_t reallocCount;
	size_t reallocDelta;
};

struct SimpleQuantityHistoryPtr
{
	const SimpleQuantityHistory * ptr;
};

void convertToJson(htopml::JsonState& json, const CallStackInfoGlobals& value);
void convertToJson(htopml::JsonState& json, const CallStackInfoRealloc& value);
void convertToJson(htopml::JsonState& json, const SimpleQuantityHistoryPtr& value);
void convert(CallStackInfoGlobals & out,const CallStackInfo & in);
void convert(CallStackInfoRealloc & out,const CallStackInfo & in);

void convertToV2Tree( MALTV2::RLockFreeTree& tree, MALT::StackSTLHashMap< MALT::CallStackInfo >& orig );

}

#endif //MALT_CONVERT_TO_V2_TREE_HPP

/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_CONVERT_TO_V2_TREE_HPP
#define MALT_CONVERT_TO_V2_TREE_HPP

/********************  HEADERS  *********************/
//standard
#include "./AllocStackProfiler.hpp"
#include "./../stack-tree/from-v2/RLockFreeTree.hpp"

/*******************  NAMESPACE  ********************/
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

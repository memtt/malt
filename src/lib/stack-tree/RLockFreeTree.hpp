/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_STACK_STL_HASH_HPP
#define MATT_STACK_STL_HASH_HPP

/********************  HEADERS  *********************/
//std
#include <map>
#include <cassert>
//internal common
#include <common/STLInternalAllocator.hpp>
//internal stacks
#include <stacks/Stack.hpp>
#include <core/SymbolResolver.hpp>
#include <json/JsonState.h>

/*******************  NAMESPACE  ********************/
namespace MATT
{

struct RLockFreeTreeEntry
{
	RLockFreeTreeEntry * next;
	RLockFreeTreeEntry * parent;
	void * callSite;
	void * data;
};
	
class RLockFreeTree
{
	public:
		RLockFreeTree(bool threadSafe);
		RLockFreeTreeEntry * getRoot(void);
	private:
		RLockFreeTreeEntry * next;
		RLockFreeTreeEntry * next;
		Spinlock lock;
		bool threadSafe;
};


}

#endif //MATT_STACK_STL_HASH_HPP

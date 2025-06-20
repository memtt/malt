/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/stack-tree/from-v2/StackTreeMap.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2015
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALTV2_STACK_TREE_MAP_HPP
#define MALTV2_STACK_TREE_MAP_HPP

/**********************************************************/
#include "StackTree.hpp"
#include "EnterExitStack.hpp"
#include "BacktraceStack.hpp"
#include "RLockFreeTree.hpp"
#include <common/STLInternalAllocator.hpp>
#include <json/JsonState.h>


/**********************************************************/
namespace MALTV2
{

/**********************************************************/
class StackTreeMap : public StackTree
{
	public:
		struct Key 
		{
			Key(const Stack* stack, int id);
			void cloneStack(void);
			bool operator == (const Key & node) const;
			bool operator < (const Key & node) const;
			/** Keep ref to the stack. **/
			const Stack * stack;
			/** Keep the hash for fast compare on search. **/
			StackHash hash;
			/** uniq id for json output **/
			int dataId;
		};
		struct Handler 
		{
			BacktraceStack backtraceStack;
			EnterExitStack enterExitStack;
			StackTreeStorage * storage;
			int stackId;
			
		};
		typedef std::pair<const Key,StackTreeStorage> Node;
		typedef std::map<Key,StackTreeStorage,std::less<Key>,MALT::STLInternalAllocator< Node > > NodeMap;
	public:
		StackTreeMap(bool backtrace = true,bool threadSafe = true);
		~StackTreeMap(void);
		virtual StackTreeHandler enterFunction(StackTreeHandler handler,MALT::LangAddress callsite);
		virtual StackTreeHandler exitFunction(StackTreeHandler handler,MALT::LangAddress callsite);
		virtual StackTreeHandler enterThread(void);
		virtual void exitThread(StackTreeHandler handler);
		virtual StackTreeHandler getFromStack(StackTreeHandler handler, const Stack & stack);
		virtual StackTreeHandler getFromStack(StackTreeHandler handler, int skip);
		virtual bool isEnterExit ( void ) const;
		virtual void toJson ( htopml::JsonState& json, const StackTree& tree ) const;
		virtual StackTreeDataHandler getDataHandler ( StackTreeHandler handler );
		virtual int getStackId ( StackTreeDataHandler handler );
		virtual void prepareForOutput ( void );
		virtual void registerSymbols ( SymbolRegistry& registry ) const;
	public:
		friend void convertToJson(htopml::JsonState & json, const StackTreeMap & tree);
	protected:
		virtual void * & internalGetData(StackTreeHandler handler,int id);
		virtual void * getData(StackTreeHandler handler,int id);
		virtual void* getData ( StackTreeDataHandler handler, int id );
		void printData(htopml::JsonState & json,int id) const;
	protected:
		MALT::Spinlock lock;
		bool threadSafe;
		bool backtrace;
		NodeMap map;
		int nextId;
};

}

#endif //MALTV2_STACK_TREE_MAP_HPP

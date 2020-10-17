/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALTV2_STACK_TREE_MAP_HPP
#define MALTV2_STACK_TREE_MAP_HPP

/********************  HEADERS  *********************/
#include "StackTree.hpp"
#include "EnterExitStack.hpp"
#include "BacktraceStack.hpp"
#include "RLockFreeTree.hpp"
#include <common/STLInternalAllocator.hpp>
#include <json/JsonState.h>


/*******************  NAMESPACE  ********************/
namespace MALTV2
{

/*********************  CLASS  **********************/
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
		virtual StackTreeHandler enterFunction(StackTreeHandler handler, void* callsite);
		virtual StackTreeHandler exitFunction(StackTreeHandler handler,void  * callsite);
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

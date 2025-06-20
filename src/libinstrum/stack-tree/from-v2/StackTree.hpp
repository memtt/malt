/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/stack-tree/from-v2/StackTree.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2015 - 2018
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALTV2_STACK_TREE_HPP
#define MALTV2_STACK_TREE_HPP

/**********************************************************/
#include "Stack.hpp"
#include <cassert>
#include <json/JsonState.h>
#include <common/NoFreeAllocator.hpp>

/**********************************************************/
namespace MALTV2
{

/**********************************************************/
#define MALT_STACK_TREE_ENTRIES 8
#define MALT_STACK_TREE_NULL NULL

/**********************************************************/
class StackTreeStorage
{
	public:
		StackTreeStorage() {for (int i = 0 ; i < MALT_STACK_TREE_ENTRIES ; i++) data[i] = NULL;};
		void * & operator[](int id) {assert(id >= 0 && id < MALT_STACK_TREE_ENTRIES); return data[id];};
		void * operator[](int id) const {assert(id >= 0 && id < MALT_STACK_TREE_ENTRIES); return (data[id]);};
	private:
		void* data [MALT_STACK_TREE_ENTRIES];
};

/**********************************************************/
class StackTreeTypeDescriptor
{
	public:
		virtual ~StackTreeTypeDescriptor(void) {};
		virtual void * allocate(void) = 0;
		virtual void deallocate(void * ptr) = 0;
		virtual void toJson(htopml::JsonState & json,const char * name,void * ptr) = 0;
		virtual size_t typeSize(void) = 0;
};

/**********************************************************/
template <class T>
class StackTreeTypeDescriptorTyped : public StackTreeTypeDescriptor
{
	public:
		virtual void * allocate(void) {return MALT_NO_FREE_NEW(T);};
		virtual void deallocate(void * ptr) {delete (T*)ptr;};
		virtual void toJson(htopml::JsonState & json,const char * name,void * ptr) {json.printField(name,*(T*)ptr);};
		virtual size_t typeSize(void) { return sizeof (T); };
};

/**********************************************************/
typedef void * StackTreeHandler;
typedef StackTreeStorage * StackTreeDataHandler;
typedef int StackId;

/**********************************************************/
class StackTree
{
	public:
		StackTree(void){
			descriptorsCnt = 0;
			for (int i = 0 ; i < MALT_STACK_TREE_ENTRIES ; i++)
				descriptors[i] = NULL;
		};
		~StackTree(void){};
		virtual StackTreeHandler enterFunction(StackTreeHandler handler,MALT::LangAddress callsite) = 0;
		virtual StackTreeHandler exitFunction(StackTreeHandler handler,MALT::LangAddress callsite) = 0;
		virtual StackTreeHandler enterThread(void) = 0;
		virtual StackTreeHandler getFromStack(StackTreeHandler handler,const Stack & stack) = 0;
		virtual StackTreeHandler getFromStack(StackTreeHandler handler,int skip) = 0;
		virtual void exitThread(StackTreeHandler handler) = 0;
		virtual bool isEnterExit(void) const = 0;
		virtual StackTreeDataHandler getDataHandler(StackTreeHandler handler) = 0;
		virtual StackId getStackId(StackTreeDataHandler handler) = 0;
		virtual void prepareForOutput(void) = 0;
		template <class T> T & getTypedData(StackTreeHandler handler,int id){assert(sizeof(T) == descriptors[id]->typeSize()); return *(T*)getData(handler,id);}
		template <class T> T & getTypedData(StackTreeDataHandler handler,int id){assert(sizeof(T) == descriptors[id]->typeSize()); return *(T*)getData(handler,id);}
		template <class T> int addDescriptor(const std::string & name) {return addDescriptor(name,new StackTreeTypeDescriptorTyped<T>());}
		int addDescriptor(const std::string name, StackTreeTypeDescriptor * descriptor) {names[descriptorsCnt]=name;descriptors[descriptorsCnt]=descriptor;return descriptorsCnt++;}
		virtual void toJson(htopml::JsonState & json, const StackTree & tree) const = 0;
		virtual void registerSymbols(SymbolRegistry & solver) const = 0;
		friend void convertToJson(htopml::JsonState & json, const StackTree & tree) {tree.toJson(json,tree);}
	protected:
		virtual void * getData(StackTreeHandler handler,int id) = 0;
		virtual void * getData(StackTreeDataHandler handler,int id)  = 0;
// 		virtual void setData(StackTreeHandler handler,int id, void* data) = 0;
	protected:
		std::string names[MALT_STACK_TREE_ENTRIES];
		StackTreeTypeDescriptor * descriptors[MALT_STACK_TREE_ENTRIES];
		int descriptorsCnt;
};

}

#endif //MALTV2_STACK_TREE_HPP

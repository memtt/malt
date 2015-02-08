/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_STACK_TREE_HPP
#define MATT_STACK_TREE_HPP
#include "Stack.hpp"
#include <json/JsonState.h>

/********************  HEADERS  *********************/

/*******************  NAMESPACE  ********************/
namespace MATT
{

typedef void * StackTreeHandler;

/********************  MACROS  **********************/
#define MATT_STACK_TREE_ENTRIES 2

/*********************  CLASS  **********************/
class StackTreeTypeDescriptor
{
	public:
		virtual ~StackTreeTypeDescriptor(void) {};
		virtual void * allocate(void) = 0;
		virtual void deallocate(void * ptr) = 0;
		virtual void toJson(htopml::JsonState & json,const char * name,void * ptr) = 0;
};

/*********************  CLASS  **********************/
template <class T>
class StackTreeTypeDescriptorTyped : public StackTreeTypeDescriptor
{
	public:
		virtual void * allocate(void) {return new T;};
		virtual void deallocate(void * ptr) {delete (T*)ptr;};
		virtual void toJson(htopml::JsonState & json,const char * name,void * ptr) {json.printField(name,*(T*)ptr);};
};

/*********************  CLASS  **********************/
class StackTree
{
	public:
		StackTree(void){
			descriptorsCnt = 0;
			for (int i = 0 ; i < MATT_STACK_TREE_ENTRIES ; i++)
				descriptors[i] = NULL;
		};
		~StackTree(void){};
		virtual StackTreeHandler enterFunction(StackTreeHandler handler,void * callsite) = 0;
		virtual StackTreeHandler exitFunction(StackTreeHandler handler,void  * callsite) = 0;
		virtual StackTreeHandler enterThread(void) = 0;
		virtual StackTreeHandler setFromStack(StackTreeHandler handler,const Stack & stack) = 0;
		virtual void exitThread(StackTreeHandler handler) = 0;
		template <class T> T & getTypedData(StackTreeHandler handler,int id){return *(T*)getData(handler,id);};
		template <class T> int addDescriptor(const std::string & name) {names[descriptorsCnt]=name;descriptors[descriptorsCnt]=new StackTreeTypeDescriptorTyped<T>();return descriptorsCnt++;};
	protected:
		virtual void * getData(StackTreeHandler handler,int id) = 0;
		virtual void setData(StackTreeHandler handler,int id, void* data) = 0;
	protected:
		std::string names[MATT_STACK_TREE_ENTRIES];
		StackTreeTypeDescriptor * descriptors[MATT_STACK_TREE_ENTRIES];
		int descriptorsCnt;
};

}

#endif //MATT_STACK_TREE_HPP

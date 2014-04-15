/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_STACK_TREE_SUB_SET_HPP
#define MATT_STACK_TREE_SUB_SET_HPP

/********************  HEADERS  *********************/
#include <common/Debug.hpp>
#include <json/ConvertToJson.h>
#include <json/JsonState.h>
#include "StackTreeSubSetNode.hpp"
#include <core/Stack.h>

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  CLASS  **********************/
template <class T>
class StackTreeSubSet
{
	public:
		typedef StackTreeSubSetNode<T> * Handler;
	public:
		StackTreeSubSet(void);
		Handler buildNewhandler(void);
		void destroyHandler(Handler & handler);
		void enter(Handler & handler,void * childAddr);
		void exit(Handler & handler,void * curAddr);
		void moveToBacktrace(Handler & handler);
		T & getData(Handler & handler);
	public:
		template <class U> friend void convertToJson(htopml::JsonState & json, const StackTreeSubSet<U> & value);
	private:
		StackTreeSubSetNode<T> rootNode;
};

/*******************  FUNCTION  *********************/
template <class T>
StackTreeSubSet<T>::StackTreeSubSet(void)
	:rootNode(NULL,NULL)
{

}

/*******************  FUNCTION  *********************/
template <class T>
typename StackTreeSubSet<T>::Handler StackTreeSubSet<T>::buildNewhandler(void)
{
	return &rootNode;
}

/*******************  FUNCTION  *********************/
template <class T>
void StackTreeSubSet<T>::enter(typename StackTreeSubSet<T>::Handler &handler, void* addr)
{
	//check
	MATT_ASSERT(handler != NULL);

	//search
	StackTreeSubSetNode<T> entry(handler,addr);
	typename std::set<StackTreeSubSetNode<T> >::iterator it = handler->childs.find(entry);
	
	//create if not found
	if (it == handler->childs.end())
	{
		handler->childs.insert(entry);
		it = handler->childs.find(entry);
	}
	
	//return
	handler = (StackTreeSubSetNode<T>*)&(*it);
}

/*******************  FUNCTION  *********************/
template <class T>
void StackTreeSubSet<T>::StackTreeSubSet::exit(typename StackTreeSubSet<T>::Handler& handler, void* curAddr)
{
	//check
	MATT_ASSERT(handler != NULL);
	MATT_ASSERT(handler != &rootNode);
	
	//move to parent
	handler = handler->parent;
}

/*******************  FUNCTION  *********************/
template <class T>
T & StackTreeSubSet<T>::getData(typename StackTreeSubSet<T>::Handler & handler)
{
	//check
	MATT_ASSERT(handler != NULL);
	
	//get res
	T * res = handler->value;

	//allocate if need
	if (res == NULL)
	{
		res = new T;
		handler->value = res;
	}

	return *res;
}

/*******************  FUNCTION  *********************/
template <class T>
void StackTreeSubSet<T>::destroyHandler(typename StackTreeSubSet<T>::Handler& handler)
{
	//nohting to to for this implementation as we only point tree nodes
}

/*******************  FUNCTION  *********************/
template <class T>
void StackTreeSubSet<T>::moveToBacktrace(typename StackTreeSubSet<T>::Handler& handler)
{
	MATT_FATAL("Backtrace currently not supported for enter-exit optimized trees !");
}

/*******************  FUNCTION  *********************/
template <class T>
void convertToJson(htopml::JsonState& json, const std::set<StackTreeSubSetNode<T> > & value)
{
	for (typename std::set<StackTreeSubSetNode<T> >::const_iterator it = value.begin() ; it != value.end() ; ++it)
	{
		if (it->value != NULL)
			json.printValue(*it);
		convertToJson(json,it->childs);
	}
}

/*******************  FUNCTION  *********************/
template <class T>
void convertToJson(htopml::JsonState& json, const MATT::StackTreeSubSet< T >& value)
{
	json.openArray();
	convertToJson(json,value.rootNode.childs);
	json.closeArray();
}

}

#endif //MATT_STACK_TREE_SUB_SET_HPP

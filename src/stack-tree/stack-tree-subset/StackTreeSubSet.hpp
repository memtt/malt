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

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  CLASS  **********************/
template <class T>
class StackTreeSubSet
{
	public:
		typedef void * Handler;
	public:
		StackTreeSubSet(void);
		Handler buildNewhandler(void);
		void destroyHandler(Handler & handler);
		void enterHandler(Handler & handler,void * childAddr);
		void exitHandler(Handler & handler,void * curAddr);
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
typename StackTreeSubSet<T>::Node StackTreeSubSet<T>::getRootNode(void)
{
	return &rootNode;
}

/*******************  FUNCTION  *********************/
template <class T>
typename StackTreeSubSet<T>::Node StackTreeSubSet<T>::getNode(typename StackTreeSubSet<T>::Node parent, void* addr)
{
	//check
	MATT_ASSERT(parent != NULL);

	//search
	StackTreeSubSetNode<T> entry(parent,addr);
	typename std::set<StackTreeSubSetNode<T> >::iterator it = parent->childs.find(entry);
	
	//create if not found
	if (it == parent->childs.end())
	{
		parent->childs.insert(entry);
		it = parent->childs.find(entry);
	}
	
	//return
	return (StackTreeSubSetNode<T>*)&(*it);
}

/*******************  FUNCTION  *********************/
template <class T>
T & StackTreeSubSet<T>::getData(typename StackTreeSubSet<T>::Node node)
{
	//setup types
	MATT_ASSERT(node != NULL);
	
	//get res
	T * res = node->value;

	//allocate if need
	if (res == NULL)
	{
		res = new T;
		node->value = res;
	}

	return *res;
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

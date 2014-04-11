/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_TREE_CALL_TRACK_HPP
#define MATT_TREE_CALL_TRACK_HPP

/********************  HEADERS  *********************/
#include <common/Debug.hpp>
#include <json/ConvertToJson.h>
#include <json/JsonState.h>
#include "TreeStackTracerEntryV3.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  CLASS  **********************/
template <class T>
class TreeStackTracerV3
{
	public:
		typedef TreeStackTracerEntryV3<T> * Node;
	public:
		TreeStackTracerV3(void);
		Node getRootNode(void);
		Node getNode(Node parentNode, void* addr);
		T & getData(Node node);
	public:
		template <class U> friend void convertToJson(htopml::JsonState & json, const TreeStackTracerV3<U> & value);
	private:
		TreeStackTracerEntryV3<T> rootNode;
};

/*******************  FUNCTION  *********************/
template <class T>
TreeStackTracerV3<T>::TreeStackTracerV3(void)
	:rootNode(NULL,NULL)
{

}

/*******************  FUNCTION  *********************/
template <class T>
typename TreeStackTracerV3<T>::Node TreeStackTracerV3<T>::getRootNode(void)
{
	return &rootNode;
}

/*******************  FUNCTION  *********************/
template <class T>
typename TreeStackTracerV3<T>::Node TreeStackTracerV3<T>::getNode(typename TreeStackTracerV3<T>::Node parent, void* addr)
{
	//check
	MATT_ASSERT(parent != NULL);

	//search
	TreeStackTracerEntryV3<T> entry(parent,addr);
	typename std::set<TreeStackTracerEntryV3<T> >::iterator it = parent->childs.find(entry);
	
	//create if not found
	if (it == parent->childs.end())
	{
		parent->childs.insert(entry);
		it = parent->childs.find(entry);
	}
	
	//return
	return (TreeStackTracerEntryV3<T>*)&(*it);
}

/*******************  FUNCTION  *********************/
template <class T>
T & TreeStackTracerV3<T>::getData(typename TreeStackTracerV3<T>::Node node)
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
void convertToJson(htopml::JsonState& json, const std::set<TreeStackTracerEntryV3<T> > & value)
{
	for (typename std::set<TreeStackTracerEntryV3<T> >::const_iterator it = value.begin() ; it != value.end() ; ++it)
	{
		if (it->value != NULL)
			json.printValue(*it);
		convertToJson(json,it->childs);
	}
}

/*******************  FUNCTION  *********************/
template <class T>
void convertToJson(htopml::JsonState& json, const MATT::TreeStackTracerV3< T >& value)
{
	json.openArray();
	convertToJson(json,value.rootNode.childs);
	json.closeArray();
}

}

#endif //MATT_TREE_CALL_TRACK_HPP

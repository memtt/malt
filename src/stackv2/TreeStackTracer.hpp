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
#include <set>
#include <common/Debug.hpp>
#include <json/ConvertToJson.h>
#include <json/JsonState.h>
#include "TreeStackTracerEntry.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  CLASS  **********************/
template <class T>
class TreeStackTracer
{
	public:
		typedef std::set< TreeStackTracerEntry<T> > TreeStackTracerEntryMap;
		typedef TreeStackTracerEntry<T> * Node;
	public:
		Node getRootNode(void);
		Node getNode(Node parentNode, void* addr);
		T & getData(Node node);
	public:
		template <class U> friend void convertToJson(htopml::JsonState & json, const TreeStackTracer<U> & value);
	private:
		TreeStackTracerEntryMap entries;
};

/*******************  FUNCTION  *********************/
template <class T>
typename TreeStackTracer<T>::Node TreeStackTracer<T>::getRootNode(void)
{
	return getNode(NULL,NULL);
}

/*******************  FUNCTION  *********************/
template <class T>
typename TreeStackTracer<T>::Node TreeStackTracer<T>::getNode(typename TreeStackTracer<T>::Node parent, void* addr)
{
	TreeStackTracerEntry<T> entry(parent,addr);
	typename TreeStackTracer<T>::TreeStackTracerEntryMap::iterator it = entries.find(entry);
	if (it == entries.end())
	{
		entries.insert(entry);
		it = entries.find(entry);
	}
	
	return (TreeStackTracerEntry<T>*)&(*it);
}

/*******************  FUNCTION  *********************/
template <class T>
T & TreeStackTracer<T>::getData(typename TreeStackTracer<T>::Node node)
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
void convertToJson(htopml::JsonState& json, const MATT::TreeStackTracer< T >& value)
{
	json.openArray();
	
	//search all non null data
	for (typename TreeStackTracer<T>::TreeStackTracerEntryMap::const_iterator it = value.entries.begin() ; it != value.entries.end() ; ++it)
	{
		if (it->value != NULL)
			json.printValue(*it);
	}
	
	json.closeArray();
}

}

#endif //MATT_TREE_CALL_TRACK_HPP

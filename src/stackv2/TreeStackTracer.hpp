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
#include <map>
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
		typedef std::map<TreeStackTracerEntry,T*> TreeStackTracerEntryMap;
		typedef typename TreeStackTracerEntryMap::iterator Node;
	public:
		Node getRootNode(void);
		Node getNode(Node & parentNode, void* addr);
		T & getData(Node & node);
	public:
		template <class U> friend void convertToJson(htopml::JsonState & json, const TreeStackTracer<U> & value);
	private:
		TreeStackTracerEntryMap entries;
};

/*******************  FUNCTION  *********************/
template <class T>
typename TreeStackTracer<T>::Node TreeStackTracer<T>::getRootNode(void)
{
	TreeStackTracerEntry entry(NULL,NULL);
	typename TreeStackTracer<T>::TreeStackTracerEntryMap::iterator it = entries.find(entry);
	if (it == entries.end())
	{
		entries[entry] = NULL;
		it = entries.find(entry);
	}
	
	return it;
}

/*******************  FUNCTION  *********************/
template <class T>
typename TreeStackTracer<T>::Node TreeStackTracer<T>::getNode(typename TreeStackTracer<T>::Node & parent, void* addr)
{
	TreeStackTracerEntry entry(&parent->first,addr);
	typename TreeStackTracer<T>::TreeStackTracerEntryMap::iterator it = entries.find(entry);
	if (it == entries.end())
	{
		entries[entry] = NULL;
		it = entries.find(entry);
	}
	
	return it;
}

/*******************  FUNCTION  *********************/
template <class T>
T & TreeStackTracer<T>::getData(typename TreeStackTracer<T>::Node & node)
{
	if (node->second == NULL)
		node->second = new T;
	return *node->second;
}

/*******************  FUNCTION  *********************/
template <class T>
void convertToJson(htopml::JsonState& json, const typename std::pair<const TreeStackTracerEntry*,const T *> & it)
{
	json.openStruct();
	const TreeStackTracerEntry * cur = it.first;
	json.openFieldArray("stack");
	while (cur != NULL)
	{
		if (cur->codeAddr != NULL)
			json.printValue(cur->codeAddr);
		cur = cur->parent;
	}
	json.closeFieldArray("stack");
	json.printField("infos",*it.second);
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
template <class T>
void convertToJson(htopml::JsonState& json, const MATT::TreeStackTracer< T >& value)
{
	json.openArray();
	
	//search all non null data
	for (typename TreeStackTracer<T>::TreeStackTracerEntryMap::const_iterator it = value.entries.begin() ; it != value.entries.end() ; ++it)
		if (it->second != NULL)
			json.printValue(std::pair<const TreeStackTracerEntry*,const T *>(&it->first,it->second));
	
	json.closeArray();
}

}

#endif //MATT_TREE_CALL_TRACK_HPP

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
#include "TreeStackTracerEntry.hpp"

/*********************  CLASS  **********************/
template <class T>
class TreeStackNode : private std::pair<TreeStackTracerEntry,T>
{
	public:
		TreeStackTracerEntry & getStack(void) {return this->first;};
		T & getStack(void) {return this->second;};
};

/*********************  CLASS  **********************/
template <class T>
class TreeStackTracer
{
	public:
		typedef ::TreeStackNode<T> Node;
		typedef std::map<TreeStackTracerEntry,T> TreeStackTracerEntryMap;
	public:
		Node & getEntry(TreeStackTracerEntry * parent,void * addr);
	public:
		friend void convertToJson(htopml::JsonState & json, const TreeStackTracer<T> & value);
	private:
		TreeStackTracerEntryMap entries;
};

/*******************  FUNCTION  *********************/
template <class T>
TreeStackTracer<T>::Node& TreeStackTracer<T>::getEntry(TreeStackTracerEntry* parent, void* addr)
{
	TreeStackTracerEntry entry(parent,addr);
	TreeStackTracerEntryMap::iterator it = entries.find(entry);
	if (it == entries.end())
	{
		T tmp;
		entries[entry] = tmp;
		it = entries.find(entry);
	}
	return static_cast<TreeStackTracer<T>::Node>(it);
}

/*******************  FUNCTION  *********************/
template <class T>
void TreeStackTracer<T>::convertToJson(htopml::JsonState& json, const TreeStackTracer< T >& value)
{
	json.openStruct();
	json.closeStruct();
}

#endif //MATT_TREE_CALL_TRACK_HPP

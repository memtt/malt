/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_STACK_STL_HASH_HPP
#define MATT_STACK_STL_HASH_HPP

/********************  HEADERS  *********************/
//std
#include <map>
#include <cassert>
//internal common
#include <common/STLInternalAllocator.hpp>
//internal stacks
#include <stacks/Stack.hpp>
#include <json/JsonState.h>

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  CLASS  **********************/
template <class T>
class StackSTLHashMap
{
	public:
		struct Key {
			Key(const Stack * stack);
			bool operator == (const Key & node) const;
			bool operator < (const Key & node) const;
			const Stack * stack;
			StackHash hash;
		};
		typedef typename std::pair<const Key,T> Node;
		typedef typename std::map<Key,T,std::less<Key>,STLInternalAllocator< Node > > InternalMap;
		typedef typename InternalMap::iterator iterator;
		typedef typename InternalMap::const_iterator const_iterator;
	public:
		StackSTLHashMap(void);
		virtual ~StackSTLHashMap(void);
		T & getValueRef(const Stack & stack);
		T & operator[](const Stack & stack);
		Node & getNode(const MATT::Stack& stack);
		iterator begin();
		iterator end();
		const_iterator begin() const;
		const_iterator end() const;
	public:
		template <class U> friend void convertToJson(htopml::JsonState & json, const StackSTLHashMap<U> & value);
	private:
		InternalMap map;
};

/*******************  FUNCTION  *********************/
template <class T>
StackSTLHashMap<T>::StackSTLHashMap(void)
{

}

/*******************  FUNCTION  *********************/
template <class T>
StackSTLHashMap<T>::~StackSTLHashMap(void)
{

}

/*******************  FUNCTION  *********************/
template <class T>
typename StackSTLHashMap<T>::Node & StackSTLHashMap<T>::getNode(const Stack& stack)
{
	//build key
	Key key(&stack);
	
	//search
	typename InternalMap::iterator it = map.find(key);
	
	//if not found, create
	if (it == map.end())
	{
		map[key] = T();
		it = map.find(key);
	}
	
	//return
	return (*it);
}

/*******************  FUNCTION  *********************/
template <class T>
T& StackSTLHashMap<T>::getValueRef(const Stack& stack)
{
	return getNode(stack).second;
}

/*******************  FUNCTION  *********************/
template <class T>
T& StackSTLHashMap<T>::operator[](const Stack& stack)
{
	return getNode(stack).second;
}

/*******************  FUNCTION  *********************/
template <class T>
StackSTLHashMap<T>::Key::Key(const Stack* stack)
{
	assert(stack != NULL);
	this->stack = stack;
	this->hash = stack->hash();
}

/*******************  FUNCTION  *********************/
template <class T>
bool StackSTLHashMap<T>::Key::operator==(const Key& node) const
{
	if (!(hash == node.hash))
		return false;
	else
		return *stack == *node.stack;
}

/*******************  FUNCTION  *********************/
template <class T>
bool StackSTLHashMap<T>::Key::operator<(const Key& node) const
{
	if (hash < node.hash)
	{
		return true;
	} else if (hash == node.hash) {
		return *stack < *node.stack;
	} else {
		return false;
	}
}

/*******************  FUNCTION  *********************/
template <class U>
void convertToJson(htopml::JsonState& json, const StackSTLHashMap< U >& value)
{
	json.openStruct();
	json.openFieldArray("stats");
	for (typename StackSTLHashMap< U >::InternalMap::const_iterator it = value.map.begin() ; it != value.map.end() ; ++it)
	{
		json.printListSeparator();
		json.openStruct();
		json.printField("stack",*it->first.stack);
		json.printField("infos",it->second);
		json.closeStruct();
	}
	json.closeFieldArray("stats");
	json.printField("count",value.map.size());
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
template <class T>
typename StackSTLHashMap<T>::iterator StackSTLHashMap<T>::begin()
{
	return map.begin();
}

/*******************  FUNCTION  *********************/
template <class T>
typename StackSTLHashMap<T>::iterator StackSTLHashMap<T>::end()
{
	return map.end();
}

/*******************  FUNCTION  *********************/
template <class T>
typename StackSTLHashMap<T>::const_iterator StackSTLHashMap<T>::begin() const
{
	return map.begin();
}

/*******************  FUNCTION  *********************/
template <class T>
typename StackSTLHashMap<T>::const_iterator StackSTLHashMap<T>::end() const
{
	return map.end();
}

}

#endif //MATT_STACK_STL_HASH_HPP

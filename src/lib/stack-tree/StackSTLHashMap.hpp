/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_STACK_STL_HASH_HPP
#define MALT_STACK_STL_HASH_HPP

/********************  HEADERS  *********************/
//std
#include <map>
#include <cassert>
//from htpoml for json export
#include <json/JsonState.h>
//internal common
#include <common/STLInternalAllocator.hpp>
//internal stacks
#include <stacks/Stack.hpp>
#include <core/SymbolSolver.hpp>

/*******************  NAMESPACE  ********************/
namespace MALT
{

/********************  GLOBALS  *********************/
//to help unit test implementation
extern unsigned long gblUseFakeIdInOutput;

/*********************  CLASS  **********************/
/**
 * Provide a simple stack storage based on STL hash map. We use a three level hash construct 
 * to quicly find the entries without reading the whole stack content to find it in map.
 * 
 * The key is composed in priority order :
 *      - A simple hash of the map sotred into an unsigned long. Each stack maintain this hash for fast compare.
 *      - Depth of the stack.
 *      - Detailed content of the stack.
 * 
 * The ordering operator (<) will only compare the next  key if previous one provide equal reqults with
 * the compared stack.
**/
template <class T>
class StackSTLHashMap
{
	public:
		/** Quicly define the key formed by pointer to the stack and the hash **/
		struct Key {
			Key(const Stack * stack);
			void cloneStack(void);
			bool operator == (const Key & node) const;
			bool operator < (const Key & node) const;
			/** Keep ref to the stack. **/
			const Stack * stack;
			/** Keep the hash for fast compare on search. **/
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
		Node & getNode(const MALT::Stack& stack);
		iterator begin();
		iterator end();
		const_iterator begin() const;
		const_iterator end() const;
		void solveSymbols(SymbolSolver & symbolResolver);
		bool empty() const;
		void remove(iterator & it);
	public:
		template <class U> friend void convertToJson(htopml::JsonState & json, const StackSTLHashMap<U> & value);
	private:
		/** Instal STL map. **/
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
		key.cloneStack();
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
		if (gblUseFakeIdInOutput > 0) //for unit tests
			json.printField("stackId",(void*)(gblUseFakeIdInOutput++));
		else
			json.printField("stackId",(void*)it->first.stack);
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

/*******************  FUNCTION  *********************/
template <class T>
void StackSTLHashMap<T>::solveSymbols(SymbolSolver& symbolResolver)
{
	for (typename StackSTLHashMap< T >::InternalMap::const_iterator it = map.begin() ; it != map.end() ; ++it)
		it->first.stack->solveSymbols(symbolResolver);
}

/*******************  FUNCTION  *********************/
template <class T>
void StackSTLHashMap<T>::Key::cloneStack(void)
{
	assert(stack != NULL);
	void * ptr = MALT_MALLOC(sizeof(Stack));
	stack = new(ptr) Stack(*stack);
}

/*******************  FUNCTION  *********************/
template <class T>
bool StackSTLHashMap<T>::empty() const
{
	return map.empty();
}

/*******************  FUNCTION  *********************/
template <class T>
void StackSTLHashMap<T>::remove ( iterator& it )
{
	this->map.erase(it);
}

}

#endif //MALT_STACK_STL_HASH_HPP

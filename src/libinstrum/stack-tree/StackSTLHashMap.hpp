/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 09/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/stack-tree/StackSTLHashMap.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2015
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_STACK_STL_HASH_HPP
#define MALT_STACK_STL_HASH_HPP

/**********************************************************/
//std
#include <map>
#include <unordered_map>
#include <cassert>
//from htpoml for json export
#include <json/JsonState.h>
//internal common
#include <common/STLInternalAllocator.hpp>
//internal stacks
#include <stacks/Stack.hpp>
#include <core/SymbolSolver.hpp>

/**********************************************************/
namespace MALT
{

/** Quicly define the key formed by pointer to the stack and the hash **/
template <bool doStackClone = true>
struct StackKey {
	StackKey(const Stack * stack);
	void cloneStack(void);
	bool operator == (const StackKey<doStackClone> & node) const;
	bool operator < (const StackKey<doStackClone> & node) const;
	/** Keep ref to the stack. **/
	const Stack * stack;
	/** Keep the hash for fast compare on search. **/
	StackHash hash;
};

}

/**********************************************************/
template<bool doStackClone>
struct std::hash<MALT::StackKey<doStackClone> > {
    std::size_t operator()(MALT::StackKey<doStackClone> const& s) const noexcept {
        return s.hash;
    }
};

/**********************************************************/
namespace MALT
{

/**********************************************************/
//to help unit test implementation
extern unsigned long gblUseFakeIdInOutput;

/**********************************************************/
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
template <class T, bool doCloneStack = true>
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
		//typedef typename std::pair<const StackKey<doCloneStack>,T> Node;
		//typedef typename std::map<Key,T,std::less<Key>,STLInternalAllocator< Node > > InternalMap;
		typedef typename std::pair<const StackKey<doCloneStack> ,T> Node;
		typedef typename std::unordered_map<StackKey<doCloneStack>,T,std::hash<StackKey<doCloneStack> >,std::equal_to<StackKey<doCloneStack> >,STLInternalAllocator< Node > > InternalMap;
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
		iterator find(const Stack & stack);
		void erase(const Stack & stack);
		const_iterator begin() const;
		const_iterator end() const;
		const_iterator find(const Stack & stack) const;
		void solveSymbols(SymbolSolver & symbolResolver);
		bool empty() const;
		void remove(iterator & it);
	public:
		template <class U, bool doCloneStackU> friend void convertToJson(htopml::JsonState & json, const StackSTLHashMap<U, doCloneStackU> & value);
	private:
		/** Instal STL map. **/
		InternalMap map;
};

/**********************************************************/
template <class T, bool doCloneStack>
StackSTLHashMap<T, doCloneStack>::StackSTLHashMap(void)
{

}

/**********************************************************/
template <class T, bool doCloneStack>
StackSTLHashMap<T, doCloneStack>::~StackSTLHashMap(void)
{

}

/**********************************************************/
template <class T, bool doCloneStack>
typename StackSTLHashMap<T, doCloneStack>::Node & StackSTLHashMap<T, doCloneStack>::getNode(const Stack& stack)
{
	//build key
	StackKey<doCloneStack> key(&stack);
	
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

/**********************************************************/
template <class T, bool doCloneStack>
T& StackSTLHashMap<T, doCloneStack>::getValueRef(const Stack& stack)
{
	return getNode(stack).second;
}

/**********************************************************/
template <class T, bool doCloneStack>
T& StackSTLHashMap<T, doCloneStack>::operator[](const Stack& stack)
{
	return getNode(stack).second;
}

/**********************************************************/
template<bool T>
inline StackKey<T>::StackKey(const Stack* stack)
{
	assert(stack != NULL);
	this->stack = stack;
	this->hash = stack->hash();
}

/**********************************************************/
template<bool T>
inline bool StackKey<T>::operator==(const StackKey& node) const
{
	if (!(hash == node.hash))
		return false;
	else
		return *stack == *node.stack;
}

/**********************************************************/
template<bool T>
inline bool StackKey<T>::operator<(const StackKey& node) const
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

/**********************************************************/
template <class U, bool doCloneStackU = true>
void convertToJson(htopml::JsonState& json, const StackSTLHashMap< U, doCloneStackU >& value)
{
	json.openStruct();
	json.openFieldArray("stats");
	for (const auto & it : value.map)
	{
		json.printListSeparator();
		json.openStruct();
		json.printField("stack",*it.first.stack);
		if (gblUseFakeIdInOutput > 0) //for unit tests
			json.printField("stackId",(void*)(gblUseFakeIdInOutput++));
		else
			json.printField("stackId",(void*)it.first.stack);
		json.printField("infos",it.second);
		json.closeStruct();
	}
	json.closeFieldArray("stats");
	json.printField("count",value.map.size());
	json.closeStruct();
}

/**********************************************************/
template <class T, bool doCloneStack>
typename StackSTLHashMap<T, doCloneStack>::iterator StackSTLHashMap<T, doCloneStack>::find(const Stack & stack)
{
	//build key
	StackKey<doCloneStack> key(&stack);

	//search
	return map.find(key);
}

/**********************************************************/
template <class T, bool doCloneStack>
void StackSTLHashMap<T, doCloneStack>::erase(const Stack & stack)
{
	//build key
	StackKey<doCloneStack> key(&stack);

	//erase
	map.erase(key);
}

/**********************************************************/
template <class T, bool doCloneStack>
typename StackSTLHashMap<T, doCloneStack>::const_iterator StackSTLHashMap<T, doCloneStack>::find(const Stack & stack) const
{
	//build key
	StackKey<doCloneStack> key(&stack);

	//search
	return map.find(key);
}

/**********************************************************/
template <class T, bool doCloneStack>
typename StackSTLHashMap<T, doCloneStack>::iterator StackSTLHashMap<T, doCloneStack>::begin()
{
	return map.begin();
}

/**********************************************************/
template <class T, bool doCloneStack>
typename StackSTLHashMap<T, doCloneStack>::iterator StackSTLHashMap<T, doCloneStack>::end()
{
	return map.end();
}

/**********************************************************/
template <class T, bool doCloneStack>
typename StackSTLHashMap<T, doCloneStack>::const_iterator StackSTLHashMap<T, doCloneStack>::begin() const
{
	return map.begin();
}

/**********************************************************/
template <class T, bool doCloneStack>
typename StackSTLHashMap<T, doCloneStack>::const_iterator StackSTLHashMap<T, doCloneStack>::end() const
{
	return map.end();
}

/**********************************************************/
template <class T, bool doCloneStack>
void StackSTLHashMap<T, doCloneStack>::solveSymbols(SymbolSolver& symbolResolver)
{
	for (typename StackSTLHashMap< T >::InternalMap::const_iterator it = map.begin() ; it != map.end() ; ++it)
		it->first.stack->solveSymbols(symbolResolver);
}

/**********************************************************/
template<bool doCloneStack>
void StackKey<doCloneStack>::cloneStack(void)
{
	if (doCloneStack) {
		assert(stack != NULL);
		void * ptr = MALT_MALLOC(sizeof(Stack));
		stack = new(ptr) Stack(*stack);
	}
}

/**********************************************************/
template <class T, bool doCloneStack>
bool StackSTLHashMap<T, doCloneStack>::empty() const
{
	return map.empty();
}

/**********************************************************/
template <class T, bool doCloneStack>
void StackSTLHashMap<T, doCloneStack>::remove ( iterator& it )
{
	this->map.erase(it);
}

}

#endif //MALT_STACK_STL_HASH_HPP

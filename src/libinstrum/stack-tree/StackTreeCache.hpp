/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 09/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/stack-tree/StackTreeCache.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_STACK_TREE_CACHE_HPP
#define MALT_STACK_TREE_CACHE_HPP

/**********************************************************/
#include <config.h>
#include <cassert>
#include <map>
#include <vector>
#include "StackSTLHashMap.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
/**
 * @brief Define a cache based on tree indexing
 * 
 * The goal it to fast find values in a tree by interposing a partial tree
 * with limied number of member to get a fast search in.
 */
template <class Value>
class StackTreeCache
{
	public:
		StackTreeCache(size_t size);
		~StackTreeCache(void);
		const Value * get(const Stack & key) const;
		void set(const Stack * key, const Value & value);
		void flush(void);
		void printStats(const std::string & name) const;
	private:
		StackSTLHashMap<Value, false> dict;
		std::vector<const Stack *> keyHistory;
		size_t size{0};
		size_t cursor{0};
		mutable size_t hits{0};
		mutable size_t misses{0};
		size_t flushes{0};
		bool dirty{true};
};

/**********************************************************/
template <class Value>
StackTreeCache<Value>::StackTreeCache(size_t size)
{
	this->size = size;
	this->keyHistory.reserve(size);
	for (size_t i = 0 ; i < size ; i++)
		this->keyHistory.push_back(nullptr);
}

/**********************************************************/
template <class Value>
StackTreeCache<Value>::~StackTreeCache(void)
{
}

/**********************************************************/
template <class Value>
const Value * StackTreeCache<Value>::get(const Stack & key) const
{
	//search in tree
	const auto & it = this->dict.find(key);

	//check
	if (it != this->dict.end()) {
		#ifdef MALT_ENABLE_CODE_TIMING
			this->hits++;
		#endif //MALT_ENABLE_CODE_TIMING
		return &it->second;
	} else {
		#ifdef MALT_ENABLE_CODE_TIMING
			this->misses++;
		#endif //MALT_ENABLE_CODE_TIMING
		return nullptr;
	}
}

/**********************************************************/
template <class Value>
void StackTreeCache<Value>::set(const Stack * key, const Value & value)
{
	//check
	assert(key != nullptr);

	//remove one
	if (this->keyHistory[this->cursor] != nullptr) {
		this->dict.erase(*this->keyHistory[this->cursor]);
		this->keyHistory[this->cursor] = nullptr;
	}

	//set it
	this->dict[*key] = value;
	this->keyHistory[this->cursor] = key;

	//incr with wrapping
	this->cursor = (this->cursor + 1) % this->size;
}

/**********************************************************/
template <class Value>
void StackTreeCache<Value>::flush(void)
{
	for (auto & it : this->dict)
		this->dict.remove(it);
	for (size_t i = 0 ; i < this->size ; i++)
		this->keyHistory[i] = nullptr;
	#ifdef MALT_ENABLE_CODE_TIMING
		this->flush++;
	#endif //MALT_ENABLE_CODE_TIMING
}

/**********************************************************/
template <class Value>
void StackTreeCache<Value>::printStats(const std::string & name) const
{
	printf("%s cache hits: %zu, miss: %zu, ratio: %f, flush: %zu\n",name.c_str(),hits,misses,(float)hits/((float)hits+(float)misses),flushes);
}

}

#endif //MALT_STACK_TREE_CACHE_HPP

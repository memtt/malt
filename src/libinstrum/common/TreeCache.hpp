/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/common/StringIdDictionnary.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_TREE_CACHE_HPP
#define MALT_TREE_CACHE_HPP

/**********************************************************/
#include <config.h>
#include <cassert>
#include <map>
#include <vector>

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
template <class Key, class Value>
class TreeCache
{
	const size_t cstFallbackToSmall = 4;
	public:
		TreeCache(size_t size);
		~TreeCache(void);
		const Value * get(const Key & key) const;
		void set(const Key * key, const Value & value);
		void flush(void);
		void printStats(const std::string & name) const;
	private:
		std::map<Key, Value, std::less<Key>, STLInternalAllocator<std::pair<Key, Value> > > dict;
		std::vector<const Key *> keyHistory;
		std::vector<Value> directSmall;
		size_t size{0};
		size_t cursor{0};
		mutable size_t hits{0};
		mutable size_t misses{0};
		size_t flushes{0};
		bool dirty{true};
};

/**********************************************************/
template <class Key, class Value>
TreeCache<Key, Value>::TreeCache(size_t size)
{
	this->size = size;
	this->keyHistory.reserve(size);
	if (size <= cstFallbackToSmall) {
		directSmall.reserve(size);
		for (size_t i = 0 ; i < size ; i++)
			directSmall.emplace_back();
	}
	for (size_t i = 0 ; i < size ; i++)
		this->keyHistory.push_back(nullptr);
}

/**********************************************************/
template <class Key, class Value>
TreeCache<Key, Value>::~TreeCache(void)
{
}

/**********************************************************/
template <class Key, class Value>
const Value * TreeCache<Key, Value>::get(const Key & key) const
{
	if (this->size <= cstFallbackToSmall) {
		for (size_t i = 0 ; i < size ; i++) {
			if (this->keyHistory[i] != nullptr && *this->keyHistory[i] == key) {
				#ifdef MALT_ENABLE_CODE_TIMING
					this->hits++;
				#endif //MALT_ENABLE_CODE_TIMING
				return &directSmall[i];
			} else {
				#ifdef MALT_ENABLE_CODE_TIMING
					this->misses++;
				#endif //MALT_ENABLE_CODE_TIMING
				return nullptr;
			}
		}
		return nullptr;
	} else {
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
}

/**********************************************************/
template <class Key, class Value>
void TreeCache<Key, Value>::set(const Key * key, const Value & value)
{
	//check
	assert(key != nullptr);

	if (this->size <= cstFallbackToSmall) {
		//set it
		this->directSmall[this->cursor] = value;
		this->keyHistory[this->cursor] = key;

		//incr with wrapping
		this->cursor = (this->cursor + 1) % this->size;
	} else {
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
}

/**********************************************************/
template <class Key, class Value>
void TreeCache<Key, Value>::flush(void)
{
	if (this->size > cstFallbackToSmall)
		for (auto & it : this->dict)
			this->dict.remove(it);
	for (size_t i = 0 ; i < this->size ; i++)
		this->keyHistory[i] = nullptr;
	#ifdef MALT_ENABLE_CODE_TIMING
		this->flush++;
	#endif //MALT_ENABLE_CODE_TIMING
}

/**********************************************************/
template <class Key, class Value>
void TreeCache<Key, Value>::printStats(const std::string & name) const
{
	printf("%s cache hits: %zu, miss: %zu, ratio: %f, flush: %zu\n",name.c_str(),hits,misses,(float)hits/((float)hits+(float)misses),flushes);
}

}

#endif //MALT_TREE_CACHE_HPP

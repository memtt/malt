/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/common/StringIdDictionnary.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
//std
#include <cassert>
#include <climits>
//internal
#include <common/CodeTiming.hpp>
#include "StringIdDictionnary.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
/**
 * @brief Constructor of the dictionnary.
 */
StringIdDictionnary::StringIdDictionnary(void)
	:cache(97)
{
}

/**********************************************************/
/**
 * @brief Translate an ID into a string.
 * 
 * @param id ID to be translated.
 * @return The string corresponding to the ID.
 */
const String & StringIdDictionnary::getString(int id) const
{
	//check
	assert((size_t)id < this->strings.size());

	//ok
	return this->strings[id];
}

/**********************************************************/
/**
 * @brief Translate a string into an ID
 * 
 * If the string is new it is added to the dictionnary with a new ID assigned.
 * 
 * @param value The string value to convert and register if new.
 * @return The ID assigned to the string value.
 */
int StringIdDictionnary::getId(const String & value)
{
	//lock
	std::lock_guard<std::mutex> guard(this->mutex);

	//use cache
	#ifdef MALT_ENABLE_CACHING
		const size_t * cacheResult = nullptr;
		CODE_TIMING("stringSearchCacheGet", cacheResult = this->cache.get(value));
		if (cacheResult != nullptr) {
			return *cacheResult;
		}
	#endif //MALT_ENABLE_CACHING

	//loop to search
	std::map<MALT::String, size_t>::iterator it;
	CODE_TIMING("stringSearch", it = this->stringToId.find(value));
	if (it != this->stringToId.end()) {
		#ifdef MALT_ENABLE_CACHING
			CODE_TIMING("stringSearchCacheSet", this->cache.set(&it->first, it->second));
		#endif //MALT_ENABLE_CACHING
		return it->second;
	}

	//not found => insert
	strings.emplace_back(value);
	assert(strings.size() < INT_MAX);
	const size_t id = strings.size() - 1;

	//set id
	this->stringToId[value] = id;

	#ifdef MALT_ENABLE_CACHING
		const auto & it2 = this->stringToId.find(value);
		this->cache.set(&it2->first, id);
	#endif //MALT_ENABLE_CACHING

	//ok
	return id;
}

/**********************************************************/
/**
 * @brief Dump the string dictionnary into json format.
 * 
 * @param json The json stread to pipe in.
 * @param value The dictionnary to dump.
 */
void convertToJson(htopml::JsonState & json, const StringIdDictionnary & value)
{
	json.openArray();
		for (const auto & it : value.strings)
			json.printValue(it, false);
	json.closeArray();
}

/**********************************************************/
void StringIdDictionnary::printStats(void) const
{
	this->cache.printStats("StringIdDictionnary");
}

}

/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/server-files/SimpleIdCache.js
*-----------------------------------------------------------
*    AUTHOR   : Mehdi Raza Jaffery (CERN) - 2016
*    AUTHOR   : Sébastien Valat (INRIA) - 2023
*    AUTHOR   : Sébastien Valat - 2024
***********************************************************/

/**********************************************************/
#include "SimpleIdCache.hpp"

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
/**
 * Checks if the object is already in the cache
 * @param  {string} value The object to check
 * @return {boolean}      True if value is in cache
 */
bool SimpleIdCache::exists(const std::string & value) const
{
	return this->cache.find(value) != this->cache.end();
}

/**********************************************************/
/**
 * Put an object into the cache and returns its unique id.
 *
 * If the object already exists in the cache, just
 * returns the id of the object.
 *
 * @param  {string} value Value to put into the cache
 * @return {int}          Unique id for this value
 */
size_t SimpleIdCache::put(const std::string & value)
{
	const auto & it = this->cache.find(value);
	if (it == this->cache.end()) {
		const size_t id = this->nextId++;
		this->cache[value] = id;
		return id;
	} else {
		return it->second;
	}
}

/**
 * Get the unique id for an object.
 *
 * If the object is not in the cache, returns false.
 *
 * @param  {string} value Value to check in the cache
 * @return {int|boolean}  Returns the unqiue id or
 *                        false if value is not found.
 */
size_t SimpleIdCache::get(const std::string & value) const
{
	const auto & it = this->cache.find(value);
	if (it == this->cache.end()) {
		return -1;
	} else {
		return it->second;
	}
}

}

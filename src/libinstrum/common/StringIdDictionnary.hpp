/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/common/StringIdDictionnary.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_STRING_DICTIONNARY_HPP
#define MALT_STRING_DICTIONNARY_HPP

/**********************************************************/
#include <string>
#include <vector>
#include <mutex>
//extern deps
#include <json/JsonState.h>
//internal
#include "String.hpp"
#include "STLInternalAllocator.hpp"
#include "common/TreeCache.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
/**
 * @brief Define a string dictionnary.
 * 
 * This dictionnary assign an ID to each string and keep track of them. This way
 * we can use the ID everywhere instead of duplicating the string. It also goes
 * faster for comparison.
 * 
 * @todo I might be able to remove the strings array which is redundant to stringToId.
 */
class StringIdDictionnary
{
	public:
		StringIdDictionnary(void);
		const String & getString(int id) const;
		int getId(const String & value);
		void printStats(void) const;
	public:
		friend void convertToJson(htopml::JsonState & json, const StringIdDictionnary & value);
	private:
		/** Keep track of the list of strings. */
		std::vector<String, STLInternalAllocator<String> > strings;
		/** Permit to get the ID from the string. */
		std::map<String, size_t, std::less<String>, STLInternalAllocator<std::pair<String, size_t> > > stringToId;
		/** Mutex to protect the access to the disctionnary so we can use it in threads. */
		std::mutex mutex;
		/** Cache for faster search on large tree */
		TreeCache<String, size_t> cache;
};

}

#endif //MALT_STRING_DICTIONNARY_HPP

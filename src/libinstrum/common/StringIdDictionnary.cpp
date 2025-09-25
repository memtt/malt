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
#include "StringIdDictionnary.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
/**
 * @brief Constructor of the dictionnary.
 */
StringIdDictionnary::StringIdDictionnary(void)
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

	//loop to search
	const auto it = this->stringToId.find(value);
	if (it != this->stringToId.end())
		return it->second;

	//not found => insert
	strings.emplace_back(value);
	assert(strings.size() < INT_MAX);
	const size_t id = strings.size() - 1;

	//set id
	this->stringToId[value] = id;

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

}

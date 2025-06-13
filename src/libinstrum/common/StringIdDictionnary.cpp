/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.0
*    DATE     : 02/2025
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
StringIdDictionnary::StringIdDictionnary(void)
{
}

/**********************************************************/
const String & StringIdDictionnary::getString(int id) const
{
	//check
	assert((size_t)id < this->strings.size());

	//ok
	return this->strings[id];
}

/**********************************************************/
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
void convertToJson(htopml::JsonState & json, const StringIdDictionnary & value)
{
	json.openArray();
		for (const auto & it : value.strings)
			json.printValue(it, false);
	json.closeArray();
}

}

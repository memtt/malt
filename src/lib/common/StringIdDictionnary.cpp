/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/common/SimpleAllocator.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
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
const std::string & StringIdDictionnary::getString(int id) const
{
	//check
	assert((size_t)id < this->strings.size());

	//ok
	return this->strings[id];
}

/**********************************************************/
int StringIdDictionnary::getId(const std::string & value)
{
	//vars
	int id = 0;

	//lock
	std::lock_guard<std::mutex> guard(this->mutex);

	//loop to search
	for (size_t i = 0 ; i < this->strings.size() ; i++)
		if (this->strings[i] == value)
			return i;
	
	//not found => insert
	strings.push_back(value);

	//ok
	assert(strings.size() < INT_MAX);
	return strings.size()-1;
}

/**********************************************************/
void convertToJson(htopml::JsonState & json, const StringIdDictionnary & value)
{
	json.printValue(value.strings);
}

}

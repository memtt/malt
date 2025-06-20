/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 02/2025
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

/**********************************************************/
namespace MALT
{

/**********************************************************/
class StringIdDictionnary
{
	public:
		StringIdDictionnary(void);
		const String & getString(int id) const;
		int getId(const String & value);
	public:
		friend void convertToJson(htopml::JsonState & json, const StringIdDictionnary & value);
	private:
		std::vector<String, STLInternalAllocator<String> > strings;
		std::map<String, size_t> stringToId;
		std::mutex mutex;
};

}

#endif //MALT_STRING_DICTIONNARY_HPP

/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/core/SegmentTracker.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2024
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
		std::mutex mutex;
};

}

#endif //MALT_STRING_DICTIONNARY_HPP

/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 03/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/format/Json.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_SELECT_JSON_IN_HPP
#define MALT_SELECT_JSON_IN_HPP

/**********************************************************/
#include <string>
#include <nlohmann/json.hpp>
#include "json/Json.hpp"
#include "json/File.hpp"

/**********************************************************/
namespace MALTFormat
{

#define USE_INTERNAL_JSON_IN
#ifdef USE_INTERNAL_JSON_IN
	typedef MALTJson::Json JsonIn;
	#define jsContains(value, field) ((value).contains(field))
	typedef MALTJson::JsonFile JsonFileIn;
#else
	typedef nlohmann::json JsonIn;
	#define jsContains(value, field) ((value).find((field)) != (value).end())
	class JsonFileIn
	{
		public:
			JsonFileIn(const std::string & fname) {this->root = nlohmann::json::parse(fname);}
			nlohmann::json getRoot(void) {return this->root;};
		private:
			nlohmann::json root;
	};
#endif

}

#endif //MALT_SELECT_JSON_IN_HPP

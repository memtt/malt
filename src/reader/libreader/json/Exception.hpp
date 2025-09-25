/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/json/Exception.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#ifndef MALT_JSON_EXCEPTION_HPP
#define MALT_JSON_EXCEPTION_HPP

/**********************************************************/
#include <stdexcept>

/**********************************************************/
namespace MALTJson
{

/**********************************************************/
class JsonException : public std::runtime_error
{
	public:
		JsonException(const std::string & what): std::runtime_error(what){};
};

}

#endif //MALT_JSON_EXCEPTION_HPP

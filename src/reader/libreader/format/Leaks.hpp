/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 03/2025
*    LICENSE  : CeCILL-C
*    FILE     : ./src/reader/libreader/format/Leaks.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_FORMAT_LEAKS_HPP
#define MALT_FORMAT_LEAKS_HPP

/**********************************************************/
#include <string>
#include <vector>
#include "Json.hpp"
#include "Types.hpp"

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
struct Leak
{
	std::vector<LangAddress> stack;
	size_t count;
	size_t memory;
};

/**********************************************************/
typedef std::vector<Leak> Leaks;

/**********************************************************/
void from_json(const JsonIn & json, Leak & value);
void to_json(nlohmann::json & json, const Leak & value);

}

#endif //MALT_FORMAT_LEAKS_HPP

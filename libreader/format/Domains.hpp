/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/Backtrace.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_FORMAT_DOMAINS_HPP
#define MALT_FORMAT_DOMAINS_HPP

/**********************************************************/
#include <string>
#include <vector>
#include "Json.hpp"
#include "Types.hpp"

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
struct DomainCounters
{
	size_t c;
	size_t pyObj;
	size_t pyMem;
	size_t pyRaw;
};

/**********************************************************/
struct Domains
{
	DomainCounters counters;
	DomainCounters mem;
};

/**********************************************************/
void from_json(const JsonIn & json, DomainCounters & value);
void from_json(const JsonIn & json, Domains & value);
void to_json(nlohmann::json & json, const DomainCounters & value);
void to_json(nlohmann::json & json, const Domains & value);

}

#endif //MALT_FORMAT_DOMAINS_HPP

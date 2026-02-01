/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 10/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/format/Domains.hpp
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
	size_t mmap;
	size_t gpu;
};

/**********************************************************/
struct Domains
{
	DomainCounters counters;
	DomainCounters mem;
	DomainCounters atGlobalPeak;
	DomainCounters localPeak;
};

/**********************************************************/
void from_json(const JsonIn & json, DomainCounters & value);
void from_json(const JsonIn & json, Domains & value);
void to_json(nlohmann::json & json, const DomainCounters & value);
void to_json(nlohmann::json & json, const Domains & value);

}

#endif //MALT_FORMAT_DOMAINS_HPP

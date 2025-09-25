/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/format/Types.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_FORMAT_TYPES_INFOS_HPP
#define MALT_FORMAT_TYPES_INFOS_HPP

/**********************************************************/
#include <string>
#include "Json.hpp"

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
typedef unsigned long long CyclesTime;
typedef unsigned long long CyclesDuration;
typedef void * StackId;

/**********************************************************/
enum Lang
{
	LANG_C,
	LANG_PYTHON,
	LANG_TRANS_PTR,
};

/**********************************************************/
struct LangAddress
{
	Lang lang;
	void * address;
};

/**********************************************************/
bool operator<(const LangAddress & a, const LangAddress & b);
bool operator==(const LangAddress & a, const LangAddress & b);

/**********************************************************/
std::string to_string(const LangAddress & address);
LangAddress to_lang_address(const std::string & value);

/**********************************************************/
void from_json(const JsonIn & json, LangAddress & value);
void to_json(nlohmann::json & json, const LangAddress & value);
void from_json(const JsonIn & json, void* & ptr);
void to_json(nlohmann::json & json, const void * & ptr);

}

#endif //MALT_FORMAT_TYPES_INFOS_HPP

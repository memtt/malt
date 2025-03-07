/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/Backtrace.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_FORMAT_TYPES_INFOS_HPP
#define MALT_FORMAT_TYPES_INFOS_HPP

/**********************************************************/
#include <string>

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
};

/**********************************************************/
struct LangAddress
{
	Lang lang;
	void * address;
};

}

#endif //MALT_FORMAT_TYPES_INFOS_HPP

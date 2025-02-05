/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.5
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/common/Helpers.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

/**********************************************************/
//standard
#include <cassert>
#include <iomanip>
#include <cstdio>
//portability wrappers
#include <portability/OS.hpp>
//header to implement
#include "Helpers.hpp"

/**********************************************************/
using namespace std;

/**********************************************************/
namespace MALT
{

/**********************************************************/
/** Strings for unit printing. **/
static const char * cstUnits[] = {"","K","M","G","T","P"};

/**********************************************************/
/**
 * Help to format values by using adapted unit.
 * @param value The value to print.
 * @param unit The name of the unit to use (cycles, seconds...). Empty for none.
**/
void Helpers::printValue(std::ostream & out,double value, const char* unit)
{
	//errors
	assert(unit != NULL);
	char buffer[16];

	//compte depth
	int depth = 0;
	while (value >= 1000)
	{
		depth++;
		value /= 1000.0;
	}
	
	sprintf(buffer,"%3.1f",value);
	
	//print
	out << setw(5) << std::right << buffer << " " << setw(1) << cstUnits[depth] << unit;
}

/**********************************************************/
/**
 * Return a numeric identifier to add to output filename. By default it uses the current
 * PID but it can also be override by LD_PRELOAD to support MPI ranks.
**/
int Helpers::getFileId(void )
{
	return OS::getPID();
}

}

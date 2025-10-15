/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 09/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/common/Helpers.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
//standard
#include <cassert>
#include <iomanip>
#include <cstdio>
#include <unistd.h>
#include <sstream>
//portability wrappers
#include <portability/OS.hpp>
#include <portability/Visibility.hpp>
#include "Debug.hpp"
#include "String.hpp"
//header to implement
#include "Helpers.hpp"

/**********************************************************/
using namespace std;

/**********************************************************/
namespace MALT
{

/**********************************************************/
/** Strings for unit printing. **/
static const char * cstUnits[] = {"","K","M","G","T","P","E","Y"};

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
DLL_PUBLIC int Helpers::getFileId(void )
{
	return OS::getPID();
}

/**********************************************************/
/**
 * Tells if the file ID comes from a rank or not.
 */
DLL_PUBLIC bool Helpers::fileIdIsRank(void)
{
	return true;
}

/**********************************************************/
/**
 * @brief Load a file in a single call to help writing some unit tests.
 * @param fname Define the name of the file to be loaded.
 */
std::string Helpers::loadFullFile(const std::string & fname)
{
	//output
	std::string result;

	//open
	FILE * fp = fopen(fname.c_str(), "r");
	if (fp == NULL)
		return result;

	//load
	while (!feof(fp)) {
		char buffer[4096];
		ssize_t size = fread(buffer, 1,sizeof(buffer)-1, fp);
		buffer[size] = '\0';
		result += buffer;
	}

	//close
	fclose(fp);

	//ok
	return result;
}

/**********************************************************/
/**
 * @brief Write a file in a single call to help writting some unit tests.
 * @param fname The name of the file to create / override.
 * @param data The content to store in the file under string form.
 */
bool Helpers::writeFullFile(const std::string & fname, const std::string & data)
{
	//open
	FILE * fp = fopen(fname.c_str(), "w+");
	if (fp == NULL)
		return false;

	//write
	ssize_t res = fwrite(data.c_str(), 1, data.size(), fp);
	assert(res == data.size());

	//close
	fclose(fp);

	//ok
	return res == data.size();
}

/**********************************************************/
/**
 * @brief Convert a string with K, M, G into an absolute number.
 * 
 * Note it uses a 1024 base for the unit.
 * 
 * @param value The memory size as a string.
 * @return size_t The resulting number after converting to integer and taking in account the K, M, G.
 */
size_t Helpers::valueFromKiloMegaGiga(const std::string & value)
{
	//trivial
	if (value.empty())
		return 0;

	//vars
	float valueFloat = 0.0;
	size_t valueSizet = 0;
	char last = value.back();

	//parse
	if (last == 'T' && sscanf(value.c_str(), "%fT", &valueFloat) == 1) {
		return (valueFloat * 1024.0 * 1024.0 * 1024.0 * 1024.0);
	} else if (last == 'G' && sscanf(value.c_str(), "%fG", &valueFloat) == 1) {
		return (valueFloat * 1024.0 * 1024.0 * 1024.0);
	} else if (last == 'M' && sscanf(value.c_str(), "%fM", &valueFloat) == 1) {
		return valueFloat * 1024.0 * 1024.0;
	} else if (last == 'K' && sscanf(value.c_str(), "%fK", &valueFloat) == 1) {
		return valueFloat * 1024.0;
	} else if (last >= '0' && last <= '9' && sscanf(value.c_str(), "%zu", &valueSizet) == 1) {
		return valueSizet;
	} else {
		MALT_FATAL_ARG("Invalid format for parameter : %2").arg(value).end();
		return 0;
	}
}

/**********************************************************/
/**
 * Convert a string under the form of 1,2,3,5-8,9 as a list of
 * allowed ranks in a set.
 */
IntSet Helpers::rankStrToIntSet(const std::string & value)
{
	//var
	IntSet result;

	//had a coma so it will handle the flush of last value
	String patchedValue = value.c_str();
	patchedValue += ",";

	//trivial
	if (value.empty())
		return result;

	//check format
	for (const auto & c : value)
		if (!(c == '-' || c == ',' || (c >= '0' && c <= '9')))
			MALT_FATAL_ARG("MALT: Error: Invalid rank list format : %1").arg(value).end();

	//split & fill
	String buffer;
	String rangeStart;
	for (const auto & c : patchedValue) {
		if (c >= '0' && c <= '9') {
			buffer += c;
		} else if (c == ',' && rangeStart.empty() && buffer.empty() == false) {
			//convert & check
			const int vint = atoi(buffer.c_str());
			assumeArg(vint >= 0, "Ranks should be positive value : %1").arg(buffer).end();

			//insert & reset
			result.insert(vint);
			buffer = "";
		} else if (c == ',' && rangeStart.empty() == false && buffer.empty() == false) {
			//convert
			const int vintStart = atoi(rangeStart.c_str());
			const int vintEnd = atoi(buffer.c_str());

			//check
			assumeArg(vintStart >= 0, "Ranks should be positive value : %1").arg(buffer).end();
			assumeArg(vintEnd >= 0, "Ranks should be positive value : %1").arg(buffer).end();
			assumeArg(vintStart < vintEnd, "Rank range should be in right order : %1 - %2").arg(rangeStart).arg(buffer).end();
			assumeArg(vintEnd - vintStart < 1000, "Rank range is too wide, maybe this is a mistake : %1 - %2").arg(rangeStart).arg(buffer).end();

			//insert all
			for (int i = vintStart ; i <= vintEnd ; i++)
				result.insert(i);

			//reset
			buffer = rangeStart = "";
		} else if (c == '-' && rangeStart.empty()) {
			rangeStart = buffer;
			buffer = "";
		} else {
			MALT_FATAL_ARG("Invalid rank list format : %1").arg(value).end();
		}
	}

	//ok
	return result;
}

}

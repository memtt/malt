/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 06/2025
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
#include "Debug.hpp"
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

/**********************************************************/
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
std::string Helpers::simpleProfileDump(const std::string & profileFile, const std::string & sourceFile, bool counters)
{
	//create tmp file
	char tmpFile[] = "malt-unit-test-XXXXXX";
	int fd = mkstemp(tmpFile);
	assert(fd > 0);

	//build command
	std::stringstream cmd;
	cmd << "node "
		<< SRC_PATH << "/src/webview/malt-simple-dump.js "
		<< "-i " << profileFile << " "
		<< "-s " << sourceFile << " "
		<< "| grep -A 100000 -e '--------------------------------------------' ";
	if (counters == false)
		cmd << "| egrep '^[a-zA-Z0-9/.]' ";
	cmd << "| sed -e 's#" SRC_PATH "##g' "
		<< "> " << tmpFile;

	//run
	int status = system(cmd.str().c_str());
	if (status != 0)
		return "CONVERSION_ERROR";

	//read
	std::string result = MALT::Helpers::loadFullFile(tmpFile);

	//close & remove
	close(fd);
	unlink(tmpFile);

	//ok
	return result;
}

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

}

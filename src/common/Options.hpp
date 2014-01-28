/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_OPTIONS_HPP
#define MATT_OPTIONS_HPP

/********************  HEADERS  *********************/
#include <string>

namespace MATT
{

/********************  STRUCT  **********************/
struct Options
{
	Options(void);
	//vars
	bool doStackProfile;
	bool doTimeProfile;
	std::string outputFile;
	std::string valgrindFile;
	int timeProfilePoints;
	bool timeProfileLinear;
};

/*******************  FUNCTION  *********************/
inline Options::Options(void)
{
	this->timeProfilePoints  = 1024;
	this->outputFile         = "matt-%1-%2.json";
	this->valgrindFile       = "matt-%1-%2.callgrind";
	this->doStackProfile     = true;
	this->doTimeProfile      = true;
	this->timeProfileLinear  = false;
}

};

#endif //MATT_OPTIONS_HPP

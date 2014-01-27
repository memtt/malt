#ifndef ATT_OPTIONS_HPP
#define ATT_OPTIONS_HPP

/********************  HEADERS  *********************/
#include <string>

namespace ATT
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
	this->timeProfilePoints  = 4096;
	this->outputFile         = "att.json";
	this->valgrindFile       = "att.callgrind";
	this->doStackProfile     = true;
	this->doTimeProfile      = true;
	this->timeProfileLinear  = false;
}

};

#endif //ATT_OPTIONS_HPP

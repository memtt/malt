/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_OPTIONS_HPP
#define MALT_OPTIONS_HPP

/********************  HEADERS  *********************/
//std
#include <string>
#include <cassert>

/********************  HEADERS  *********************/
//iniparser
extern "C" {
#include <iniparser.h>
}

/*********************  TYPES  **********************/
namespace htopml {
	class JsonState;
}

/*******************  NAMESPACE  ********************/
namespace MALT
{

/********************  MACROS  **********************/
/** Disable dumping on signal **/
#define MALT_NO_DUMP_SIGNAL ""

/**********************  ENUM  **********************/
/**
 * To express MALT verbosity level.
**/
enum Verbosity
{
	MALT_VERBOSITY_SILENT,
	MALT_VERBOSITY_DEFAULT,
	MALT_VERBOSITY_VERBOSE,
	MALT_VERBOSITY_COUNT,
};

/********************  STRUCT  **********************/
/**
 * Structure to manage the MALT options. It manage loading status from a config 
 * file in INI format.
 * 
 * @brief Structure to manage all the MALT options.
**/
struct Options
{
	Options(void);
	void loadFromFile(const char * fname);
	void loadFromString(const char * value);
	void loadFromIniDic(dictionary * iniDic);
	void dumpConfig(const char * fname);
	//vars for stack profilinf
	bool stackProfileEnabled;
	bool stackResolve;
	bool stackLibunwind;
	std::string stackMode;
	int stackSkip;
	//vars for time profiging
	bool timeProfileEnabled;
	int timeProfilePoints;
	bool timeProfileLinear;
	//max stack
	bool maxStackEnabled;
	//output
	std::string outputName;
	bool outputIndent;
	bool outputLua;
	bool outputJson;
	bool outputCallgrind;
	bool outputDumpConfig;
	Verbosity outputVerbosity;
	bool outputStackTree;
	bool outputLoopSuppress;
	//size map
	bool distrAllocSize;
	bool distrReallocJump;
	bool operator==(const Options & value) const;
	//trace
	bool traceEnabled;
	//info
	bool infoHidden;
	//filter
	std::string exe;
	bool childs;
	bool enabled;
	//dump
	std::string dumpOnSignal;
	int dumpAfterSeconds;
};

/********************  GLOBALS  *********************/
/** 
 * Define a global instance of option to get access from the whole tool. 
 * Please prefer to use the accessor instead of the variable itsef directly.
 * The code will be inlined by be safer in debug mode.
**/
extern Options * gblOptions;

/*******************  FUNCTION  *********************/
static inline const Options & getGlobalOptions(void)
{
	assert(gblOptions != NULL);
	return *gblOptions;
}

/*******************  FUNCTION  *********************/
Options & initGlobalOptions(void);
Verbosity verbosityFromString(const std::string & value);
const char * verbosityToString(Verbosity value);
Verbosity iniparser_getverbosity(dictionary * d, const char * key, Verbosity notfound);
std::ostream & operator << (std::ostream & out, Verbosity value);

/*******************  FUNCTION  *********************/
/** Safer function to access to the option, with check in debug mode.**/
static inline Options & getOptions(void) 
{
	assert(gblOptions != NULL);
	return *gblOptions;
}

/*******************  FUNCTION  *********************/
/*
 * Provide some helper functions to use INI parser.
**/
struct IniParserHelper
{
	static std::string extractSectionName ( const char * key );
	static void setEntry (dictionary * dic, const char * key, const char* value );
	static void setEntry (dictionary * dic, const char * key, bool value);
	static void setEntry (dictionary * dic, const char * key, int value);
};

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState & json,const Options & value);

}

#endif //MALT_OPTIONS_HPP

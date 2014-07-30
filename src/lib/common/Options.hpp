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
namespace MATT
{

/********************  STRUCT  **********************/
struct Options
{
	Options(void);
	void loadFromFile(const char * fname);
	void dumpConfig(const char * fname);
	//vars for stack profilinf
	bool stackProfileEnabled;
	bool stackResolve;
	std::string stackMode;
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
	//size map
	bool distrAllocSize;
	bool distrReallocJump;
	bool operator==(const Options & value) const;
	//trace
	bool traceEnabled;
	//info
	bool infoHidden;
};

/*******************  FUNCTION  *********************/
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

#endif //MATT_OPTIONS_HPP

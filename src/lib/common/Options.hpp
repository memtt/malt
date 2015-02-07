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
#include <vector>
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
namespace MATT
{
	
/********************  STRUCT  **********************/
class OptionDefGeneric
{
	public:
		OptionDefGeneric(const std::string & section,const std::string & name) {this->name = name; this->section = section;};
		virtual void load(dictionary * dic) = 0;
		virtual void dump(dictionary * dic) = 0;
		virtual void dump(htopml::JsonState & json) = 0;
		virtual void setToDefault(void) = 0;
		virtual bool equal(OptionDefGeneric & ptr) const = 0;
		const std::string & getName(void) const {return name;};
		const std::string  & getSection(void) const {return section;};
	protected:
		std::string section;
		std::string name;
};

/********************  STRUCT  **********************/
template <class T>
class OptionDef : public OptionDefGeneric
{
	public:
		OptionDef(T * ptr,const std::string & section,const std::string & name,const T & defaultValue);
		void load(dictionary * dic);
		void dump(dictionary * dic);
		void dump(htopml::JsonState & json);
		void setToDefault(void);
		bool equal(OptionDefGeneric & ptr) const;
	private:
		T * ptr;
		T defaultValue;
};

/*********************  TYPES  **********************/
typedef std::vector<OptionDefGeneric*> OptionDefVector;

/********************  STRUCT  **********************/
struct Options
{
	Options(void);
	void loadFromFile(const char * fname);
	void dumpConfig(const char * fname);
	OptionDefVector options;
	//vars for stack profilinf
	bool stackProfileEnabled;
	bool stackResolve;
	std::string stackMode;
	bool stackLibunwind;
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

/*******************  FUNCTION  *********************/
/** Safer function to access to the option, with check in debug mode.**/
static inline Options & getOptions(void) 
{
	assert(gblOptions != NULL);
	return *gblOptions;
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState & json,const Options & value);

}

#endif //MATT_OPTIONS_HPP

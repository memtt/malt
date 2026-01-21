/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 10/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/common/Options.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014 - 2019
*    AUTHOR   : Sébastien Valat (CERN) - 2015
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_OPTIONS_HPP
#define MALT_OPTIONS_HPP

/**********************************************************/
//std
#include <string>
#include <map>
#include <cassert>
#include <functional>

/**********************************************************/
//iniparser
extern "C" {
	#include <iniparser.h>
}

/**********************************************************/
namespace htopml {
	class JsonState;
}

/**********************************************************/
namespace MALT
{

/**********************************************************/
/** Disable dumping on signal **/
#define MALT_NO_DUMP_SIGNAL ""

/**********************************************************/
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

/**********************************************************/
enum StackMode
{
	STACK_MODE_BACKTRACE,
	STACK_MODE_ENTER_EXIT_FUNC,
	STACK_MODE_USER,
	STACK_MODE_NONE,
	STACK_MODE_PYTHON
};

/**********************************************************/
enum PythonMode
{
	PYTHON_MODE_PROFILE,
	PYTHON_MODE_TRACE,
};

/**********************************************************/
void convertToJson(htopml::JsonState & json,const StackMode & value);
void convertToJson(htopml::JsonState & json,const PythonMode & value);
void convertToJson(htopml::JsonState & json,const Verbosity & value);

/**********************************************************/
std::ostream & operator<<(std::ostream & out, StackMode mode);
std::ostream & operator<<(std::ostream & out, PythonMode mode);

/**********************************************************/
int setByString(int & number, const std::string & value);
size_t setByString(size_t & number, const std::string & value);
StackMode setByString(StackMode & mode, const std::string & value);
PythonMode setByString(PythonMode & mode, const std::string & value);
Verbosity setByString(Verbosity & mode, const std::string & value);
}

/**********************************************************/
#include "OptionsMeta.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
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
	//stack
	struct {
		bool enabled{true};
		bool resolve{true};
		bool libunwind{false};
		StackMode mode{STACK_MODE_BACKTRACE};
		int skip{4};
	} stack;
	//addr2line
	struct {
		int bucket{350};
		int threads{8};
		size_t huge{50UL * 1024UL *1024UL};
	} addr2line;
	//smpling
	struct {
		bool enabled{false};
		int volume{4093}; //5242883, 10485767, 20971529
		int count{571};
	} sampling;
	//python
	struct {
		StackMode stack{STACK_MODE_ENTER_EXIT_FUNC};
		bool mix{false};
		bool enabled{true};
		bool obj{true};
		bool mem{true};
		bool raw{true};
		bool hideImports{true};
		PythonMode mode{PYTHON_MODE_PROFILE};
	} python;
	//c
	bool cMalloc{true};
	bool cMmap{true};
	//vars for time profiging
	bool timeProfileEnabled{true};
	int timeProfilePoints{512};
	bool timeProfileLinear{false};
	//max stack
	bool maxStackEnabled{true};
	//output
	std::string outputName{"malt-%1-%2.%3"};
	bool outputIndent{false};
	bool outputLua{false};
	bool outputJson{true};
	bool outputCallgrind{false};
	bool outputDumpConfig{false};
	Verbosity outputVerbosity{MALT_VERBOSITY_DEFAULT};
	bool outputStackTree{false};
	bool outputLoopSuppress{false};
	//size map
	bool distrAllocSize{true};
	bool distrReallocJump{true};
	//trace
	bool traceEnabled{false};
	//info
	bool infoHidden{false};
	//filter
	std::string exe{""};
	bool childs{true};
	bool enabled{true};
	std::string filterRanks{""};
	//dump
	std::string dumpOnSignal{MALT_NO_DUMP_SIGNAL};
	int dumpAfterSeconds{0};
	std::string dumpOnSysFullAt{""};
	std::string dumpOnAppUsingRss{""};
	std::string dumpOnAppUsingVirt{""};
	std::string dumpOnAppUsingReq{""};
	std::string dumpOnThreadStackUsing{""};
	std::string dumpOnAllocCount{""};
	bool dumpWatchDog{false};
	//tools
	bool toolsNm{true};
	std::string toolsNmMaxSize{"50M"};

	bool operator==(const Options & value) const;
};

/**********************************************************/
class OptionsMeta
{
	public:
		OptionsMeta(Options & value);
		~OptionsMeta(void);
		void dumpConfig(const char * name) const;
		void dumpAsJson(htopml::JsonState & json) const;
		void loadIni(dictionary* iniDic);
		void dump(std::ostream & out) const;
		bool isValidGroupKey(const std::string & value) const;
	private:
		template<class T> OptionMeta<T> add(const std::string & group, const std::string & key, T & value);
	private:
		std::map<std::string, OptionMetaBase *> meta;
};

/**********************************************************/
template<class T>
OptionMeta<T> OptionsMeta::add(const std::string & group, const std::string & key, T & value)
{
	OptionMeta<T> * option = new OptionMeta<T>(group, key, value);
	this->meta[option->getGroupKey()] = option;
	return *option;
}

/**********************************************************/
/** 
 * Define a global instance of option to get access from the whole tool. 
 * Please prefer to use the accessor instead of the variable itsef directly.
 * The code will be inlined by be safer in debug mode.
**/
extern Options * gblOptions;

/**********************************************************/
static inline const Options & getGlobalOptions(void)
{
	assert(gblOptions != NULL);
	return *gblOptions;
}

/**********************************************************/
Options & initGlobalOptions(void);
Verbosity verbosityFromString(const std::string & value);
StackMode stackModeFromString(const std::string & value);
const char * verbosityToString(Verbosity value);
Verbosity iniparser_getverbosity(dictionary * d, const char * key, Verbosity notfound);
std::ostream & operator << (std::ostream & out, Verbosity value);
bool validateOptionName(const std::string & value);
bool checkDeprecated(const std::string & value);

/**********************************************************/
/** Safer function to access to the option, with check in debug mode.**/
static inline Options & getOptions(void) 
{
	assert(gblOptions != NULL);
	return *gblOptions;
}

/**********************************************************/
void convertToJson(htopml::JsonState & json,const Options & value);

/**********************************************************/
std::ostream & operator<<(std::ostream & out, const Options & options);

}

#endif //MALT_OPTIONS_HPP

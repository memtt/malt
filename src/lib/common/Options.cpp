/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//std
#include <cstdio>
#include <cassert>
//internals
#include "Debug.hpp"
#include "Options.hpp"
#include <json/JsonState.h>

/*******************  NAMESPACE  ********************/
namespace MALT 
{

/********************  GLOBALS  *********************/
Options * gblOptions = NULL;
/**
 * To be used in LocalAllocStackProfiler to cut MALT internal function calls
 * when backtracing and stop exactly at malloc/free level.
 * Made as an option as I already had serveral time issues depending on compiler
 * inlining operations so in case it append to someone he can fix without
 * recompiling.
**/
static const int cstDefaultStackSkip = 4;

/********************  CONSTS  **********************/
/**
 * To convert MALT verbosity level.
**/
static const char * cstVerbosityLevels[MALT_VERBOSITY_COUNT] = {
	"silent",
	"default",
	"verbose"
};

/*******************  FUNCTION  *********************/
/**
 * Constructor to setup the default values for each options
**/
Options::Options(void)
{
	//stack
	this->stackProfileEnabled     = true;
	this->stackResolve            = true;
	this->stackLibunwind          = false;
	this->stackMode               = "backtrace";
	this->stackSkip               = cstDefaultStackSkip;
	//time
	this->timeProfileEnabled      = true;
	this->timeProfilePoints       = 512;
	this->timeProfileLinear       = false;
	//output
	this->outputName              = "malt-%1-%2.%3";
	this->outputIndent            = false;
	this->outputJson              = true;
	this->outputLua               = false;
	this->outputCallgrind         = false;
	this->outputDumpConfig        = false;
	this->outputVerbosity         = MALT_VERBOSITY_DEFAULT;
	this->outputStackTree         = false;
	this->outputLoopSuppress      = false;
	//max stack
	this->maxStackEnabled         = true;
	//maps
	this->distrAllocSize          = true;
	this->distrReallocJump        = true;
	//trace
	this->traceEnabled            = false;
	//info
	this->infoHidden              = false;
	//filter
	this->exe                     = "";
	this->childs                  = true;
	this->enabled                 = true;
	//dump
	this->dumpOnSignal            = MALT_NO_DUMP_SIGNAL;
	this->dumpAfterSeconds        = 0;
}

/*******************  FUNCTION  *********************/
/**
 * Manage operator == to help validation in unit test suite.
**/
bool Options::operator==(const Options& value) const
{
	//stack
	if (stackProfileEnabled != value.stackProfileEnabled) return false;
	if (stackResolve != value.stackResolve) return false;
	if (stackMode != value.stackMode) return false;
	if (stackLibunwind != value.stackLibunwind) return false;
	if (stackSkip != value.stackSkip) return false;
	//time
	if (this->timeProfileEnabled != value.timeProfileEnabled) return false;
	if (this->timeProfilePoints != value.timeProfilePoints) return false;
	if (this->timeProfileLinear != value.timeProfileLinear) return false;
	//output
	if (this->outputName != value.outputName) return false;
	if (this->outputIndent != value.outputIndent) return false;
	if (this->outputJson != value.outputJson) return false;
	if (this->outputLua != value.outputLua) return false;
	if (this->outputCallgrind != value.outputCallgrind) return false;
	if (this->outputDumpConfig != value.outputDumpConfig) return false;
	if (this->outputVerbosity != value.outputVerbosity)  return false;
	if (this->outputStackTree != value.outputStackTree) return false;
	if (this->outputLoopSuppress != value.outputLoopSuppress) return false;
	//max stack
	if (this->maxStackEnabled != value.maxStackEnabled) return false;
	//maps
	if (this->distrAllocSize != value.distrAllocSize) return false;
	if (this->distrReallocJump != value.distrReallocJump) return false;
	//trace
	if (this->traceEnabled != value.traceEnabled) return false;
	//info
	if (this->infoHidden != value.infoHidden) return false;
	//exe
	if (this->exe != value.exe) return false;
	if (this->childs != value.childs) return false;
	if (this->enabled != value.enabled) return false;
	//dump
	if (this->dumpOnSignal != value.dumpOnSignal) return false;
	if (this->dumpAfterSeconds != value.dumpAfterSeconds) return false;
	
	return true;
}

/*******************  FUNCTION  *********************/
/**
 * Load values from string, mostly to be used from MALT_OPTION environment variable.
 * 
 * It expect string format like :
 * 
 * SEC1:NAME1=VALUE1;SEC2:NAME2=VALUE2;
 * 
 * @param value Define the string to load as a config file.
**/
void Options::loadFromString ( const char* value )
{
	//trivial
	if (value == NULL)
		return;

	//create fake dictionary
	dictionary * dic = dictionary_new(10);
	
	//copy string
	char * dump = strdup(value);
	
	//loop on separators ';'
	char * cur = dump;
	while (*cur != '\0')
	{
		//remind start
		char * start = cur;
		char * sep = NULL;
		
		//search ';' or '\0'
		while (*cur != ';' && *cur != '\0')
		{
			if (*cur == '=')
				sep = cur;
			cur++;
		}
		
		//skip to next
		if (cur == start)
		{
			cur++;
			continue;
		}
		
		//is end
		bool isEnd = (*cur == '\0');
		assumeArg(sep != NULL,"Invalid string format to setup option : '%1', expect SECTION:NAME=VALUE.").arg(start).end();
		
		//cut strings
		*cur = '\0';
		*sep = '\0';
		sep++;
		
		//setup in INI
		IniParserHelper::setEntry(dic,start,sep);
		
		//move
		if (isEnd == false)
			cur++;
	}
	
	//load
	this->loadFromIniDic(dic);

	//free
	iniparser_freedict(dic);
	free(dump);
}

/*******************  FUNCTION  *********************/
/**
 * Internal function to load options from iniDic.
**/
void Options::loadFromIniDic ( dictionary* iniDic )
{
	//errors
	assert(iniDic != NULL);
	
	//load values for time profiling
	this->timeProfileEnabled  = iniparser_getboolean(iniDic,"time:enabled",this->timeProfileEnabled);
	this->timeProfilePoints   = iniparser_getint(iniDic,"time:points",this->timeProfilePoints);
	this->timeProfileLinear   = iniparser_getboolean(iniDic,"time:linear-index",this->timeProfileLinear);
	
	//load values for stack profiling
	this->stackResolve        = iniparser_getboolean(iniDic,"stack:resolve",this->stackResolve);
	this->stackProfileEnabled = iniparser_getboolean(iniDic,"stack:enabled",this->stackProfileEnabled);
	this->stackLibunwind      = iniparser_getboolean(iniDic,"stack:libunwind",this->stackLibunwind);
	this->stackMode           = iniparser_getstring(iniDic,"stack:mode",(char*)this->stackMode.c_str());
	this->stackSkip           = iniparser_getint(iniDic,"stack:skip",this->stackSkip);
	
	//load values for output
	this->outputName          = iniparser_getstring(iniDic,"output:name",(char*)this->outputName.c_str());
	this->outputIndent        = iniparser_getboolean(iniDic,"output:indent",this->outputIndent);
	this->outputJson          = iniparser_getboolean(iniDic,"output:json",this->outputJson);
	this->outputLua           = iniparser_getboolean(iniDic,"output:lua",this->outputLua);
	this->outputCallgrind     = iniparser_getboolean(iniDic,"output:callgrind",this->outputCallgrind);
	this->outputDumpConfig    = iniparser_getboolean(iniDic,"output:config",this->outputDumpConfig);
	this->outputVerbosity     = iniparser_getverbosity(iniDic,"output:verbosity",this->outputVerbosity);
	this->outputStackTree     = iniparser_getboolean(iniDic,"output:stack-tree",this->outputStackTree);
	this->outputLoopSuppress  = iniparser_getboolean(iniDic,"output:loop-suppress",this->outputLoopSuppress);
	
	//max stack
	this->maxStackEnabled     = iniparser_getboolean(iniDic,"max-stack:enabled",this->maxStackEnabled);
	
	//maps
	this->distrAllocSize      = iniparser_getboolean(iniDic,"distr:alloc-size",this->distrAllocSize);
	this->distrReallocJump    = iniparser_getboolean(iniDic,"distr:realloc-jump",this->distrReallocJump);
	
	//trace
	this->traceEnabled        = iniparser_getboolean(iniDic,"trace:enabled",this->traceEnabled);
	
	//info
	this->infoHidden          = iniparser_getboolean(iniDic,"info:hidden",this->infoHidden);

	//filter
	this->exe                 = iniparser_getstring(iniDic,"filter:exe",(char*)this->exe.c_str());
	this->childs              = iniparser_getboolean(iniDic,"filter:childs",this->childs);
	this->enabled             = iniparser_getboolean(iniDic,"filter:enabled",this->enabled);

	//dump
	this->dumpOnSignal        = iniparser_getstring(iniDic,"dump:on-signal",(char*)this->dumpOnSignal.c_str());
	this->dumpAfterSeconds    = iniparser_getint(iniDic,"dump:after-seconds",this->dumpAfterSeconds);
}

/*******************  FUNCTION  *********************/
/**
 * Function to load options from a config file in INI format.
**/
void Options::loadFromFile(const char* fname)
{
	//load ini dic
	dictionary * iniDic;
	assert(fname != NULL);
	iniDic = iniparser_load(fname);
	
	//errors
	assumeArg(iniDic != NULL,"Failed to load config file : %1 !").arg(fname);
	
	//load
	loadFromIniDic(iniDic);
	
	//free dic
	iniparser_freedict(iniDic);
	
	//TODO apply getenv MALT_OPTIONS to override here and add "envOverride" parameter to enable it from caller
}

/*******************  FUNCTION  *********************/
/**
 * Helper function to convert the options to JSON output format and dump it
 * into the MALT output profile.
**/
void convertToJson(htopml::JsonState & json,const Options & value)
{
	json.openStruct();
		json.openFieldStruct("time");
			json.printField("enabled",value.timeProfileEnabled);
			json.printField("points",value.timeProfilePoints);
			json.printField("linear",value.timeProfileLinear);
		json.closeFieldStruct("time");
		
		json.openFieldStruct("stack");
			json.printField("enabled",value.stackProfileEnabled);
			json.printField("mode",value.stackMode.c_str());
			json.printField("resolve",value.stackResolve);
			json.printField("libunwind",value.stackLibunwind);
			json.printField("stackSkip",value.stackSkip);
		json.closeFieldStruct("stack");
		
		json.openFieldStruct("output");
			json.printField("callgrind",value.outputCallgrind);
			json.printField("dumpConfig",value.outputDumpConfig);
			json.printField("index",value.outputIndent);
			json.printField("json",value.outputJson);
			json.printField("lua",value.outputLua);
			json.printField("name",value.outputName);
			json.printField("verbosity",verbosityToString(value.outputVerbosity));
			json.printField("stackTree",value.outputStackTree);
			json.printField("loopSuppress",value.outputLoopSuppress);
		json.closeFieldStruct("output");
		
		json.openFieldStruct("maxStack");
			json.printField("enabled",value.maxStackEnabled);
		json.closeFieldStruct("maxStack");
		
		json.openFieldStruct("distr");
			json.printField("allocSize",value.distrAllocSize);
			json.printField("reallocJump",value.distrReallocJump);
		json.closeFieldStruct("distr");
		
		json.openFieldStruct("info");
			json.printField("hidden",value.infoHidden);
		json.closeFieldStruct("info");

		json.openFieldStruct("filter");
			json.printField("exe",value.exe);
			json.printField("childs",value.childs);
			json.printField("enabled",value.enabled);
		json.closeFieldStruct("filter");

		json.openFieldStruct("dump");
			json.printField("onSignal", value.dumpOnSignal);
			json.printField("afterSeconds", value.dumpAfterSeconds);
		json.closeFieldStruct("dump");
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
/**
 * Helper to dump the config as INI file.
**/
void Options::dumpConfig(const char* fname)
{
	//create dic
	assert(fname != NULL);
	dictionary * dic = dictionary_new(10);
	
	//fill
	IniParserHelper::setEntry(dic,"time:enabled",this->timeProfileEnabled);
	IniParserHelper::setEntry(dic,"time:points",this->timeProfilePoints);
	IniParserHelper::setEntry(dic,"time:linear-index",this->timeProfileLinear);
	
	//stack
	IniParserHelper::setEntry(dic,"stack:enabled",this->timeProfileEnabled);
	IniParserHelper::setEntry(dic,"stack:mode",this->stackMode.c_str());
	IniParserHelper::setEntry(dic,"stack:resolve",this->stackResolve);
	IniParserHelper::setEntry(dic,"stack:libunwind",this->stackLibunwind);
	IniParserHelper::setEntry(dic,"stack:skip",this->stackSkip);
	
	//output
	IniParserHelper::setEntry(dic,"output:name",this->outputName.c_str());
	IniParserHelper::setEntry(dic,"output:lua",this->outputLua);
	IniParserHelper::setEntry(dic,"output:json",this->outputJson);
	IniParserHelper::setEntry(dic,"output:callgrind",this->outputCallgrind);
	IniParserHelper::setEntry(dic,"output:indent",this->outputIndent);
	IniParserHelper::setEntry(dic,"output:config",this->outputDumpConfig);
	IniParserHelper::setEntry(dic,"output:verbosity",verbosityToString(this->outputVerbosity));
	IniParserHelper::setEntry(dic,"output:stack-tree",this->outputStackTree);
	IniParserHelper::setEntry(dic,"output:loop-suppress",this->outputLoopSuppress);
	
	//max stack
	IniParserHelper::setEntry(dic,"max-stack:enabled",this->maxStackEnabled);
	
	//maps
	IniParserHelper::setEntry(dic,"distr:alloc-size",this->distrAllocSize);
	IniParserHelper::setEntry(dic,"distr:realloc-jump",this->distrReallocJump);
	
	//trace
	IniParserHelper::setEntry(dic,"trace:enabled",this->traceEnabled);
	
	//info
	IniParserHelper::setEntry(dic,"info:hidden",this->infoHidden);

	//exe
	IniParserHelper::setEntry(dic,"filter:exe",this->exe.c_str());
	IniParserHelper::setEntry(dic,"filter:childs",this->childs);
	IniParserHelper::setEntry(dic,"filter:enabled",this->enabled);

	//dump
	IniParserHelper::setEntry(dic,"dump:on-signal",this->dumpOnSignal.c_str());
	IniParserHelper::setEntry(dic,"dump:after-seconds",this->dumpAfterSeconds);
	
	//write
	FILE * fp = fopen(fname,"w");
	assumeArg(fp != NULL,"Failed to write dump of config file into %1 : %2 !").arg(fname).argStrErrno().end();
	iniparser_dump_ini(dic,fp);
	fclose(fp);
	
	//free
	iniparser_freedict(dic);
}

/*******************  FUNCTION  *********************/
/**
 * Internal function to split strings on ':' and extract the section name.
**/
std::string IniParserHelper::extractSectionName ( const char * key )
{
	std::string tmp;
	int i = 0;
	while (key[i] != ':' && key[i] != '\0')
		tmp += key[i++];
	return tmp;
}

/*******************  FUNCTION  *********************/
/**
 * Updat some entries of a dictionnary.
 * @param dic Define the dictionnary to update.
 * @param key Define the key to update.
 * @param value Define the value to setup for the given key.
**/
void IniParserHelper::setEntry(dictionary* dic, const char* key, const char* value)
{
	iniparser_set(dic,extractSectionName(key).c_str(),NULL);
	iniparser_set(dic,key,value);
}

/*******************  FUNCTION  *********************/
/**
 * Help set setup ini dic entries from boolean by converting them to string
 * internally.
 * @param dic Define the dictionnary to fill.
 * @param key Define the key to update (key:name)
 * @param value Define the boolean value to setup.
**/
void IniParserHelper::setEntry(dictionary* dic, const char* key, bool value)
{
	setEntry(dic,key,value?"true":"false");
}

/*******************  FUNCTION  *********************/
/**
 * Help set setup ini dic entries from integer by converting them to string
 * internally.
 * @param dic Define the dictionnary to fill.
 * @param key Define the key to update (key:name)
 * @param value Define the integer value to setup.
**/
void IniParserHelper::setEntry(dictionary* dic, const char* key, int value)
{
	char buffer[64];
	sprintf(buffer,"%d",value);
	setEntry(dic,key,buffer);
}

/*******************  FUNCTION  *********************/
/**
 * Need to be call once after malloc is available.
**/
Options& initGlobalOptions ( void )
{
	//error
	assume (gblOptions == NULL,"initGlobalOptions was used previously, gblOptions is already init ! ");
	gblOptions = new Options();
	return *gblOptions;
}

/*******************  FUNCTION  *********************/
/**
 * Convert verbosity level from string.
**/
Verbosity verbosityFromString(const std::string & value)
{
	//search match
	for (int i = 0 ; i < MALT_VERBOSITY_COUNT ; i++)
		if (value == cstVerbosityLevels[i])
			return (Verbosity)i;

	//error
	MALT_FATAL_ARG("Invalid verbosity level: '%1'").arg(value).end();
	return MALT_VERBOSITY_DEFAULT;
}

/*******************  FUNCTION  *********************/
/**
 * Convert verbosity level to string.
**/
const char * verbosityToString(Verbosity value)
{
	assert(value < MALT_VERBOSITY_COUNT);
	return cstVerbosityLevels[value];
}

/*******************  FUNCTION  *********************/
Verbosity iniparser_getverbosity(dictionary * dic, const char * key, Verbosity notFound)
{
	//tansmit
	char * tmp = iniparser_getstring(dic,key,(char*)verbosityToString(notFound));

	//convert back
	Verbosity res = verbosityFromString(tmp);

	//ok
	return res;
}

/*******************  FUNCTION  *********************/
/**
 * Dump to stream to be used for json output.
**/
std::ostream & operator << (std::ostream & out, Verbosity value)
{
	out << verbosityToString(value);
	return out;
}

}

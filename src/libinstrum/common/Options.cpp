/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 06/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/common/Options.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
*    AUTHOR   : Sébastien Valat - 2014 - 2022
*    AUTHOR   : Sébastien Valat (CERN) - 2015
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
//std
#include <cstdio>
#include <cassert>
//internals
#include "Debug.hpp"
#include "Options.hpp"
#include <json/JsonState.h>

/**********************************************************/
namespace MALT 
{

/**********************************************************/
Options * gblOptions = NULL;
/**
 * To be used in LocalAllocStackProfiler to cut MALT internal function calls
 * when backtracing and stop exactly at malloc/free level.
 * Made as an option as I already had serveral time issues depending on compiler
 * inlining operations so in case it append to someone he can fix without
 * recompiling.
**/
static const int cstDefaultStackSkip = 4;

/**********************************************************/
/**
 * To convert MALT verbosity level.
**/
static const char * cstVerbosityLevels[MALT_VERBOSITY_COUNT] = {
	"silent",
	"default",
	"verbose"
};

/**********************************************************/
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
	this->stackAddr2lineBucket    = 350;
	this->stackAddr2lineThreads   = 8;
	this->stackSampling           = false;
	this->stackSamplingBw         = 4093; //5242883, 10485767, 20971529
	//python
	this->pythonStack             = "enter-exit";
	this->pythonStackEnum         = STACK_MODE_ENTER_EXIT_FUNC;
	this->pythonMix               = false;
	this->pythonInstru            = true;
	this->pythonObj               = true;
	this->pythonMem               = true;
	this->pythonRaw               = true;
	this->pythonHideImports       = true;
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
	this->dumpOnSysFullAt         = "";
	this->dumpOnAppUsingRss       = "";
	this->dumpOnAppUsingVirt      = "";
	this->dumpOnAppUsingReq       = "";
	this->dumpOnThreadStackUsing  = "";
	this->dumpOnAllocCount        = "";
	this->dumpWatchDog            = false;
	//tools
	this->toolsNm                 = true;
	this->toolsNmMaxSize          = "50M";
}

/**********************************************************/
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
	if (stackAddr2lineBucket != value.stackAddr2lineBucket) return false;
	if (stackAddr2lineThreads != value.stackAddr2lineThreads) return false;
	if (stackSampling != value.stackSampling) return false;
	if (stackSamplingBw != value.stackSamplingBw) return false;
	//python
	if (pythonInstru != value.pythonInstru) return false;
	if (pythonStack != value.pythonStack) return false;
	if (pythonMix != value.pythonMix) return false;
	if (pythonObj != value.pythonObj) return false;
	if (pythonMem != value.pythonMem) return false;
	if (pythonRaw != value.pythonRaw) return false;
	if (pythonHideImports != value.pythonHideImports) return false;
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
	if (this->dumpOnSysFullAt != value.dumpOnSysFullAt) return false;
	if (this->dumpOnAppUsingRss != value.dumpOnAppUsingRss) return false;
	if (this->dumpOnAppUsingVirt != value.dumpOnAppUsingVirt) return false;
	if (this->dumpOnAppUsingReq != value.dumpOnAppUsingReq) return false;
	if (this->dumpOnThreadStackUsing != value.dumpOnThreadStackUsing) return false;
	if (this->dumpWatchDog != value.dumpWatchDog) return false;
	if (this->dumpOnAllocCount != value.dumpOnAllocCount) return false;
	//nm
	if (this->toolsNm != value.toolsNm) return false;
	if (this->toolsNmMaxSize != value.toolsNmMaxSize) return false;
	
	return true;
}

/**********************************************************/
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

/**********************************************************/
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
	
	//python
	this->pythonInstru        = iniparser_getboolean(iniDic,"python:instru",this->pythonInstru);
	this->pythonMix           = iniparser_getboolean(iniDic,"python:mix",this->pythonMix);
	this->pythonStack         = iniparser_getstring(iniDic,"python:stack",(char*)this->pythonStack.c_str());
	this->pythonObj           = iniparser_getboolean(iniDic,"python:obj",this->pythonObj);
	this->pythonMem           = iniparser_getboolean(iniDic,"python:mem",this->pythonMem);
	this->pythonRaw           = iniparser_getboolean(iniDic,"python:raw",this->pythonRaw);
	this->pythonStackEnum     = stackModeFromString(this->pythonStack);
	this->pythonHideImports   = iniparser_getboolean(iniDic,"python:hide-imports", this->pythonHideImports);

	//load values for stack profiling
	this->stackResolve        = iniparser_getboolean(iniDic,"stack:resolve",this->stackResolve);
	this->stackProfileEnabled = iniparser_getboolean(iniDic,"stack:enabled",this->stackProfileEnabled);
	this->stackLibunwind      = iniparser_getboolean(iniDic,"stack:libunwind",this->stackLibunwind);
	this->stackMode           = iniparser_getstring(iniDic,"stack:mode",(char*)this->stackMode.c_str());
	this->stackSkip           = iniparser_getint(iniDic,"stack:skip",this->stackSkip);
	this->stackAddr2lineBucket= iniparser_getint(iniDic,"stack:addr2lineBucket",this->stackAddr2lineBucket);
	this->stackAddr2lineThreads= iniparser_getint(iniDic,"stack:addr2lineThreads",this->stackAddr2lineThreads);
	this->stackSampling       = iniparser_getboolean(iniDic,"stack:sampling",this->stackSampling);
	this->stackSamplingBw     = iniparser_getint(iniDic,"stack:samplingBw",this->stackSamplingBw);
	
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
	this->dumpOnSysFullAt     = iniparser_getstring(iniDic,"dump:on-sys-full-at",(char*)this->dumpOnSysFullAt.c_str());
	this->dumpOnAppUsingRss   = iniparser_getstring(iniDic,"dump:on-app-using-rss",(char*)this->dumpOnAppUsingRss.c_str());
	this->dumpOnAppUsingVirt  = iniparser_getstring(iniDic,"dump:on-app-using-virt",(char*)this->dumpOnAppUsingVirt.c_str());
	this->dumpOnAppUsingReq   = iniparser_getstring(iniDic,"dump:on-app-using-req",(char*)this->dumpOnAppUsingReq.c_str());
	this->dumpOnThreadStackUsing = iniparser_getstring(iniDic,"dump:on-thread-stack-using",(char*)this->dumpOnThreadStackUsing.c_str());
	this->dumpOnAllocCount    = iniparser_getstring(iniDic,"dump:on-alloc-count",(char*)this->dumpOnAllocCount.c_str());
	this->dumpWatchDog        = iniparser_getboolean(iniDic,"dump:watch-dog",this->dumpWatchDog);

	//tools
	this->toolsNm             = iniparser_getboolean(iniDic,"tools:nm",this->toolsNm);
	this->toolsNmMaxSize      = iniparser_getstring(iniDic,"tools:nm-max-size",(char*)this->toolsNmMaxSize.c_str());
}

/**********************************************************/
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

/**********************************************************/
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
			json.printField("addr2lineBucket",value.stackAddr2lineBucket);
			json.printField("addr2lineThreads",value.stackAddr2lineThreads);
			json.printField("sampling", value.stackSampling);
			json.printField("samplingBw", value.stackSamplingBw);
		json.closeFieldStruct("stack");

		json.openFieldStruct("python");
			json.printField("instru", value.pythonInstru);
			json.printField("mix", value.pythonMix);
			json.printField("stack", value.pythonStack);
			json.printField("obj", value.pythonObj);
			json.printField("mem", value.pythonMem);
			json.printField("raw", value.pythonRaw);
			json.printField("hideImports", value.pythonHideImports);
		json.closeFieldStruct("python");
		
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
			json.printField("onSysFullAt", value.dumpOnSysFullAt);
			json.printField("onAppUsingRss", value.dumpOnAppUsingRss);
			json.printField("onAppUsingVirt", value.dumpOnAppUsingVirt);
			json.printField("onAppUsingReq", value.dumpOnAppUsingReq);
			json.printField("onThreadStackUsing", value.dumpOnThreadStackUsing);
			json.printField("onAllocCount", value.dumpOnAllocCount);
			json.printField("watchDog", value.dumpWatchDog);
		json.closeFieldStruct("dump");

		json.openFieldStruct("tools");
			json.printField("nm", value.toolsNm);
			json.printField("nmMaxSize", value.toolsNmMaxSize);
		json.closeFieldStruct("tools");
	json.closeStruct();
}

/**********************************************************/
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
	IniParserHelper::setEntry(dic,"stack:addr2lineBucket",this->stackAddr2lineBucket);
	IniParserHelper::setEntry(dic,"stack:addr2lineThreads",this->stackAddr2lineThreads);
	IniParserHelper::setEntry(dic,"stack:sampling",this->stackSampling);
	IniParserHelper::setEntry(dic,"stack:samplingBw",this->stackSamplingBw);

	//python
	IniParserHelper::setEntry(dic,"python:intru",this->pythonInstru);
	IniParserHelper::setEntry(dic,"python:stack",this->pythonStack.c_str());
	IniParserHelper::setEntry(dic,"python:mix",this->pythonMix);
	IniParserHelper::setEntry(dic,"python:obj",this->pythonObj);
	IniParserHelper::setEntry(dic,"python:mem",this->pythonMem);
	IniParserHelper::setEntry(dic,"python:raw",this->pythonRaw);
	IniParserHelper::setEntry(dic,"python:hide-imports",this->pythonHideImports);
	
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
	IniParserHelper::setEntry(dic,"dump:on-sys-full-at",this->dumpOnSysFullAt.c_str());
	IniParserHelper::setEntry(dic,"dump:on-app-using-rss",this->dumpOnAppUsingRss.c_str());
	IniParserHelper::setEntry(dic,"dump:on-app-using-virt",this->dumpOnAppUsingVirt.c_str());
	IniParserHelper::setEntry(dic,"dump:on-app-using-req",this->dumpOnAppUsingReq.c_str());
	IniParserHelper::setEntry(dic,"dump:on-thread-stack-using",this->dumpOnThreadStackUsing.c_str());
	IniParserHelper::setEntry(dic,"dump:on-alloc-count",this->dumpOnAllocCount.c_str());
	IniParserHelper::setEntry(dic,"dump:watch-dog",this->dumpWatchDog);

	//tools
	IniParserHelper::setEntry(dic,"tools:nm",this->toolsNm);
	IniParserHelper::setEntry(dic,"tools:nm-max-size",this->toolsNmMaxSize.c_str());

	//write
	FILE * fp = fopen(fname,"w");
	assumeArg(fp != NULL,"Failed to write dump of config file into %1 : %2 !").arg(fname).argStrErrno().end();
	iniparser_dump_ini(dic,fp);
	fclose(fp);
	
	//free
	iniparser_freedict(dic);
}

/**********************************************************/
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

/**********************************************************/
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

/**********************************************************/
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

/**********************************************************/
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

/**********************************************************/
/**
 * Need to be call once after malloc is available.
**/
Options& initGlobalOptions ( void )
{
	//error
	//assume (gblOptions == NULL,"initGlobalOptions was used previously, gblOptions is already init ! ");
	if (gblOptions != nullptr)
		return *gblOptions;
	gblOptions = new Options();
	return *gblOptions;
}

/**********************************************************/
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

/**********************************************************/
StackMode stackModeFromString(const std::string & value)
{
	if (value == "backtrace")
		return STACK_MODE_BACKTRACE;
	else if (value == "enter-exit")
		return STACK_MODE_ENTER_EXIT_FUNC;
	else if (value == "user")
		return STACK_MODE_USER;
	else if (value == "none")
		return STACK_MODE_NONE;
	else if (value == "python")
		return STACK_MODE_PYTHON;
	else
		MALT_FATAL_ARG("Invalid stack mode : %1").arg(value).end();

	//default
	return STACK_MODE_NONE;
}

/**********************************************************/
/**
 * Convert verbosity level to string.
**/
const char * verbosityToString(Verbosity value)
{
	assert(value < MALT_VERBOSITY_COUNT);
	return cstVerbosityLevels[value];
}

/**********************************************************/
Verbosity iniparser_getverbosity(dictionary * dic, const char * key, Verbosity notFound)
{
	//tansmit
	char * tmp = iniparser_getstring(dic,key,(char*)verbosityToString(notFound));

	//convert back
	Verbosity res = verbosityFromString(tmp);

	//ok
	return res;
}

/**********************************************************/
/**
 * Dump to stream to be used for json output.
**/
std::ostream & operator << (std::ostream & out, Verbosity value)
{
	out << verbosityToString(value);
	return out;
}

}

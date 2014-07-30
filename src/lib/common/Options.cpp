/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
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
namespace MATT 
{

/*******************  FUNCTION  *********************/
Options::Options(void)
{
	//stack
	this->stackProfileEnabled     = true;
	this->stackResolve            = true;
	this->stackMode               = "backtrace";
	//time
	this->timeProfileEnabled      = true;
	this->timeProfilePoints       = 512;
	this->timeProfileLinear       = false;
	//output
	this->outputName              = "matt-%1-%2.%3";
	this->outputIndent            = true;
	this->outputJson              = true;
	this->outputLua               = false;
	this->outputCallgrind         = false;
	this->outputDumpConfig        = true;
	//max stack
	this->maxStackEnabled         = true;
	//maps
	this->distrAllocSize          = true;
	this->distrReallocJump        = true;
}

/*******************  FUNCTION  *********************/
bool Options::operator==(const Options& value) const
{
	//stack
	if (stackProfileEnabled != value.stackProfileEnabled) return false;
	if (stackResolve != value.stackResolve) return false;
	if (stackMode != value.stackMode) return false;
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
	//max stack
	if (this->maxStackEnabled != value.maxStackEnabled) return false;
	//maps
	if (this->distrAllocSize != value.distrAllocSize) return false;
	if (this->distrReallocJump != value.distrReallocJump) return false;
	
	return true;
}

/*******************  FUNCTION  *********************/
void Options::loadFromFile(const char* fname)
{
	//load ini dic
	dictionary * iniDic;
	assert(fname != NULL);
	iniDic = iniparser_load(fname);
	
	//errors
	assumeArg(iniDic != NULL,"Failed to load config file : %1 !").arg(fname);
	
	//load values for time profiling
	this->timeProfileEnabled  = iniparser_getboolean(iniDic,"time:enabled",this->timeProfileEnabled);
	this->timeProfilePoints   = iniparser_getint(iniDic,"time:points",this->timeProfilePoints);
	this->timeProfileLinear   = iniparser_getboolean(iniDic,"time:linear_index",this->timeProfileLinear);
	
	//load values for stack profiling
	this->stackResolve        = iniparser_getboolean(iniDic,"stack:resolve",this->stackResolve);
	this->stackProfileEnabled = iniparser_getboolean(iniDic,"stack:enabled",this->stackProfileEnabled);
	this->stackMode           = iniparser_getstring(iniDic,"stack:mode",(char*)this->stackMode.c_str());
	
	//load values for output
	this->outputName          = iniparser_getstring(iniDic,"output:name",(char*)this->outputName.c_str());
	this->outputIndent        = iniparser_getboolean(iniDic,"output:indent",this->outputIndent);
	this->outputJson          = iniparser_getboolean(iniDic,"output:json",this->outputJson);
	this->outputLua           = iniparser_getboolean(iniDic,"output:lua",this->outputLua);
	this->outputCallgrind     = iniparser_getboolean(iniDic,"output:callgrind",this->outputCallgrind);
	this->outputDumpConfig    = iniparser_getboolean(iniDic,"output:config",this->outputDumpConfig);
	
	//max stack
	this->maxStackEnabled     = iniparser_getboolean(iniDic,"max-stack:enabled",this->maxStackEnabled);
	
	//maps
	this->distrAllocSize      = iniparser_getboolean(iniDic,"distr:alloc_size",this->distrAllocSize);
	this->distrReallocJump    = iniparser_getboolean(iniDic,"distr:realloc_jump",this->distrReallocJump);
	
	//free dic
	iniparser_freedict(iniDic);
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState & json,const Options & value)
{
	json.openFieldStruct("time");
		json.printField("enabled",value.timeProfileEnabled);
		json.printField("points",value.timeProfilePoints);
		json.printField("linear",value.timeProfileLinear);
	json.closeFieldStruct("time");
	
	json.openFieldStruct("stack");
		json.printField("enabled",value.stackProfileEnabled);
		json.printField("mode",value.stackMode.c_str());
		json.printField("resolve",value.stackResolve);
	json.closeFieldStruct("stack");
	
	json.openFieldStruct("output");
		json.printField("callgrind",value.outputCallgrind);
		json.printField("dumpConfig",value.outputDumpConfig);
		json.printField("index",value.outputIndent);
		json.printField("json",value.outputJson);
		json.printField("lua",value.outputLua);
		json.printField("name",value.outputName);
	json.closeFieldStruct("output");
	
	json.openFieldStruct("maxStack");
		json.printField("enabled",value.maxStackEnabled);
	json.closeFieldStruct("maxStack");
	
	json.openFieldStruct("distr");
		json.printField("allocSize",value.distrAllocSize);
		json.printField("reallocJump",value.distrReallocJump);
	json.closeFieldStruct("distr");
}

/*******************  FUNCTION  *********************/
void Options::dumpConfig(const char* fname)
{
	//create dic
	assert(fname != NULL);
	dictionary * dic = dictionary_new(10);
	
	//fill
	IniParserHelper::setEntry(dic,"time:enabled",this->timeProfileEnabled);
	IniParserHelper::setEntry(dic,"time:points",this->timeProfilePoints);
	IniParserHelper::setEntry(dic,"time:linear_index",this->timeProfileLinear);
	
	//stack
	IniParserHelper::setEntry(dic,"stack:enabled",this->timeProfileEnabled);
	IniParserHelper::setEntry(dic,"stack:mode",this->stackMode.c_str());
	IniParserHelper::setEntry(dic,"stack:resolve",this->stackResolve);
	
	//output
	IniParserHelper::setEntry(dic,"output:name",this->outputName.c_str());
	IniParserHelper::setEntry(dic,"output:lua",this->outputLua);
	IniParserHelper::setEntry(dic,"output:json",this->outputJson);
	IniParserHelper::setEntry(dic,"output:callgrind",this->outputCallgrind);
	IniParserHelper::setEntry(dic,"output:indent",this->outputIndent);
	IniParserHelper::setEntry(dic,"output:config",this->outputDumpConfig);
	
	//max stack
	IniParserHelper::setEntry(dic,"max-stack:enabled",this->maxStackEnabled);
	
	//maps
	IniParserHelper::setEntry(dic,"distr:alloc_size",this->distrAllocSize);
	IniParserHelper::setEntry(dic,"distr:realloc_jump",this->distrReallocJump);
	
	//write
	FILE * fp = fopen(fname,"w");
	assumeArg(fp != NULL,"Failed to write dump of config file into %1 : %2 !").arg(fname).argStrErrno().end();
	iniparser_dump_ini(dic,fp);
	fclose(fp);
	
	//free
	iniparser_freedict(dic);
}

/*******************  FUNCTION  *********************/
std::string IniParserHelper::extractSectionName ( const char * key )
{
	std::string tmp;
	int i = 0;
	while (key[i] != ':' && key[i] != '\0')
		tmp += key[i++];
	return tmp;
}

/*******************  FUNCTION  *********************/
void IniParserHelper::setEntry(dictionary* dic, const char* key, const char* value)
{
	iniparser_set(dic,extractSectionName(key).c_str(),NULL);
	iniparser_set(dic,key,value);
}

/*******************  FUNCTION  *********************/
void IniParserHelper::setEntry(dictionary* dic, const char* key, bool value)
{
	setEntry(dic,key,value?"true":"false");
}

/*******************  FUNCTION  *********************/
void IniParserHelper::setEntry(dictionary* dic, const char* key, int value)
{
	char buffer[64];
	sprintf(buffer,"%d",value);
	setEntry(dic,key,buffer);
}

}

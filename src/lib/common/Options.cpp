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
template <class T>
OptionDef<T>::OptionDef(MATT::Options* options, T* ptr, const std::string & section,const std::string & name, const T& defaultValue)
{
	options->options.push_back(this);
	this->ptr = ptr;
	this->section = section;
	this->name = name;
	this->defaultValue = defaultValue;
	*(this->ptr) = defaultValue;
}

/*******************  FUNCTION  *********************/
template <>
void OptionDef<bool>::dump(dictionary* dic)
{
	std::string tmp = this->section + ":" + this->name;
	IniParserHelper::setEntry(dic,tmp.c_str(),*this->ptr);
}

/*******************  FUNCTION  *********************/
template <>
void OptionDef<std::string>::dump(dictionary* dic)
{
	std::string tmp = this->section + ":" + this->name;
	IniParserHelper::setEntry(dic,tmp.c_str(),(this->ptr)->c_str());
}

/*******************  FUNCTION  *********************/
template <>
void OptionDef<int>::dump(dictionary* dic)
{
	std::string tmp = this->section + ":" + this->name;
	IniParserHelper::setEntry(dic,tmp.c_str(),(this->ptr));
}

/*******************  FUNCTION  *********************/
template <class T>
void OptionDef<T>::dump(htopml::JsonState& json)
{
	json.printField(this->name.c_str(),*this->ptr);
}

/*******************  FUNCTION  *********************/
template <class T>
bool OptionDef<T>::equal(void* ptr)
{
	return *(this->ptr) == *(T*)ptr;
}

/*******************  FUNCTION  *********************/
template <>
void OptionDef<bool>::load(dictionary* dic)
{
	std::string tmp = this->section+":"+this->name;
	*(this->ptr)  = iniparser_getboolean(dic,tmp.c_str(),*(this->ptr));
}

/*******************  FUNCTION  *********************/
template <>
void OptionDef<std::string>::load(dictionary* dic)
{
	std::string tmp = this->section+":"+this->name;
	*(this->ptr)  = iniparser_getstring(dic,tmp.c_str(),(char*)this->ptr->c_str());
}

/*******************  FUNCTION  *********************/
template <>
void OptionDef<int>::load(dictionary* dic)
{
	std::string tmp = this->section+":"+this->name;
	*(this->ptr)  = iniparser_getint(dic,tmp.c_str(),*this->ptr);
}

/*******************  FUNCTION  *********************/
template <class T>
void OptionDef<T>::setToDefault(void)
{
	*this->ptr = this->defaultValue;
}

/*******************  FUNCTION  *********************/
Options::Options(void)
{
	//stack
	new OptionDef<bool>       (this,&this->stackProfileEnabled,"stack"   ,"enabled"    ,true);
	new OptionDef<bool>       (this,&this->stackResolve       ,"stack"   ,"resolve"    ,true);
	new OptionDef<std::string>(this,&this->stackMode          ,"stack"   ,"mode"       ,"backtrace");
	//time
	new OptionDef<bool>        (this,&this->timeProfileEnabled,"time"    ,"enabled"    ,true);
	new OptionDef<int>         (this,&this->timeProfilePoints ,"time"    ,"points"     ,512);
	new OptionDef<bool>        (this,&this->timeProfileLinear ,"time"    ,"linear"     ,false);
	//output
	new OptionDef<std::string> (this,&this->outputName        ,"output"   ,"name"      ,"matt-%1-%2.%3");
	new OptionDef<bool>        (this,&this->outputIndent      ,"output"   ,"indent"    ,true);
	new OptionDef<bool>        (this,&this->outputJson        ,"output"   ,"json"      ,true);
	new OptionDef<bool>        (this,&this->outputLua         ,"output"   ,"lua"       ,false);
	new OptionDef<bool>        (this,&this->outputCallgrind   ,"output"   ,"callgrind" ,false);
	new OptionDef<bool>        (this,&this->outputDumpConfig  ,"output"   ,"config"    ,true);
	//max stack
	new OptionDef<bool>        (this,&this->maxStackEnabled   ,"max-stack","enabled"   ,true);
	//maps
	new OptionDef<bool>        (this,&this->distrAllocSize    ,"distr"    ,"alloc_size",true);
	new OptionDef<bool>        (this,&this->distrAllocSize    ,"distr"    ,"realloc_jump",true);
	//trace
	new OptionDef<bool>        (this,&this->traceEnabled      ,"trace"    ,"enabled"    ,true);
	//info
	new OptionDef<bool>        (this,&this->infoHidden        ,"info"     ,"hidden"     ,false);
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
	//trace
	if (this->traceEnabled != value.traceEnabled) return false;
	//info
	if (this->infoHidden != value.infoHidden) return false;
	
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
	
	//trace
	this->traceEnabled        = iniparser_getboolean(iniDic,"trace:enabled",this->traceEnabled);
	
	//info
	this->infoHidden          = iniparser_getboolean(iniDic,"info:hidden",this->infoHidden);
	
	//free dic
	iniparser_freedict(iniDic);
}

/*******************  FUNCTION  *********************/
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
		
		json.openFieldStruct("info");
			json.printField("hidden",value.infoHidden);
		json.closeFieldStruct("info");
	json.closeStruct();
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
	
	//trace
	IniParserHelper::setEntry(dic,"trace:enabled",this->traceEnabled);
	
	//info
	IniParserHelper::setEntry(dic,"info:hidden",this->infoHidden);
	
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

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
	this->outputLua               = true;
	this->outputCallgrind         = true;
	this->outputDumpConfig        = true;
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
	
	//free dic
	iniparser_freedict(iniDic);
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

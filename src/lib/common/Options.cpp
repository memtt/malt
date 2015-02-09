/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//std
#include <cstdio>
#include <cassert>
#include <deque>
//internals
#include "Debug.hpp"
#include "Options.hpp"
#include <json/JsonState.h>

/*******************  NAMESPACE  ********************/
namespace MATT 
{

/********************  GLOBALS  *********************/
Options * gblOptions = NULL;

/*******************  FUNCTION  *********************/
template <class T>
OptionDef<T>::OptionDef(T* ptr, const std::string & section,const std::string & name, const T& defaultValue)
:OptionDefGeneric(section,name)
{
	this->ptr = ptr;
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
bool OptionDef<T>::equal(MATT::OptionDefGeneric& ptr) const
{
	assert(this->getName() == ptr.getName() && this->getSection() == ptr.getSection());
	OptionDef<T> * tmp = dynamic_cast<OptionDef<T>*>(&ptr);;
	return *(this->ptr) == *(T*)(tmp->ptr);
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
	options.push_back(new OptionDef<bool>        (&this->stackProfileEnabled,"stack"   ,"enabled"    ,true));
	options.push_back(new OptionDef<bool>        (&this->stackResolve       ,"stack"   ,"resolve"    ,true));
	options.push_back(new OptionDef<std::string> (&this->stackMode          ,"stack"   ,"mode"       ,"backtrace"));
	options.push_back(new OptionDef<bool>        (&this->stackLibunwind     ,"stack"   ,"libunwind"  ,false));
	//time
	options.push_back(new OptionDef<bool>        (&this->timeProfileEnabled,"time"    ,"enabled"    ,true));
	options.push_back(new OptionDef<int>         (&this->timeProfilePoints ,"time"    ,"points"     ,512));
	options.push_back(new OptionDef<bool>        (&this->timeProfileLinear ,"time"    ,"linear"     ,false));
	//output
	options.push_back(new OptionDef<std::string> (&this->outputName        ,"output"   ,"name"      ,"matt-%1-%2.%3"));
	options.push_back(new OptionDef<bool>        (&this->outputIndent      ,"output"   ,"indent"    ,true));
	options.push_back(new OptionDef<bool>        (&this->outputJson        ,"output"   ,"json"      ,true));
	options.push_back(new OptionDef<bool>        (&this->outputLua         ,"output"   ,"lua"       ,false));
	options.push_back(new OptionDef<bool>        (&this->outputCallgrind   ,"output"   ,"callgrind" ,false));
	options.push_back(new OptionDef<bool>        (&this->outputDumpConfig  ,"output"   ,"config"    ,true));
	//max stack
	options.push_back(new OptionDef<bool>        (&this->maxStackEnabled   ,"max-stack","enabled"   ,true));
	//maps
	options.push_back(new OptionDef<bool>        (&this->distrAllocSize    ,"distr"    ,"alloc_size",true));
	options.push_back(new OptionDef<bool>        (&this->distrAllocSize    ,"distr"    ,"realloc_jump",true));
	//trace
	options.push_back(new OptionDef<bool>        (&this->traceEnabled      ,"trace"    ,"enabled"    ,true));
	//info
	options.push_back(new OptionDef<bool>        (&this->infoHidden        ,"info"     ,"hidden"     ,false));
}

/*******************  FUNCTION  *********************/
bool Options::operator==(const Options& value) const
{
	for (int i = 0 ; i < options.size() ; i++)
		if (options[i]->equal(*value.options[i]) == false)
			return false;
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
	
	for (int i = 0 ; i < options.size() ; i++)
		options[i]->load(iniDic);
	
	//free dic
	iniparser_freedict(iniDic);
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState & json,const Options & value)
{
	//gen group list
	std::deque<std::string> groups;
	for (int i = 0 ; i < value.options.size() ; i++)
		groups.push_back(value.options[i]->getSection());
	
	json.openStruct();
	for (std::deque<std::string>::iterator it = groups.begin() ; it != groups.end() ; ++it)
	{
		json.openFieldStruct(it->c_str());
		for (int i = 0 ; i < value.options.size() ; i++)
			value.options[i]->dump(json);
		json.closeFieldStruct(it->c_str());
		
	}
}

/*******************  FUNCTION  *********************/
void Options::dumpConfig(const char* fname)
{
	//create dic
	assert(fname != NULL);
	dictionary * dic = dictionary_new(10);
	
	for (int i = 0 ; i < options.size() ; i++)
		options[i]->dump(dic);
	
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

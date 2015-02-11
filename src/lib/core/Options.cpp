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
#include <common/Debug.hpp>
#include "Options.hpp"
#include <json/JsonState.h>
#include <allocators/NoFreeAllocator.hpp>

/*******************  NAMESPACE  ********************/
namespace MATT 
{

/********************  GLOBALS  *********************/
Options * gblOptions = NULL;

/*******************  FUNCTION  *********************/
template <class T>
OptionDef<T>::OptionDef(T* ptr, const char * section,const char * name, const T& defaultValue)
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
	char buffer[1024] = {'\0'};
	strcat(buffer,this->section);
	strcat(buffer,":");
	strcat(buffer,this->name);
	IniParserHelper::setEntry(dic,buffer,*this->ptr);
}

/*******************  FUNCTION  *********************/
template <>
void OptionDef<std::string>::dump(dictionary* dic)
{
	char buffer[1024] = {'\0'};
	strcat(buffer,this->section);
	strcat(buffer,":");
	strcat(buffer,this->name);
	IniParserHelper::setEntry(dic,buffer,(this->ptr)->c_str());
}

/*******************  FUNCTION  *********************/
template <>
void OptionDef<int>::dump(dictionary* dic)
{
	char buffer[1024] = {'\0'};
	strcat(buffer,this->section);
	strcat(buffer,":");
	strcat(buffer,this->name);
	IniParserHelper::setEntry(dic,buffer,(this->ptr));
}

/*******************  FUNCTION  *********************/
template <class T>
void OptionDef<T>::dump(htopml::JsonState& json)
{
	json.printField(this->name,*this->ptr);
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
	char buffer[1024] = {'\0'};
	strcat(buffer,this->section);
	strcat(buffer,":");
	strcat(buffer,this->name);
	*(this->ptr)  = iniparser_getboolean(dic,buffer,*(this->ptr));
}

/*******************  FUNCTION  *********************/
template <>
void OptionDef<std::string>::load(dictionary* dic)
{
	char buffer[1024] = {'\0'};
	strcat(buffer,this->section);
	strcat(buffer,":");
	strcat(buffer,this->name);
	*(this->ptr)  = iniparser_getstring(dic,buffer,(char*)this->ptr->c_str());
}

/*******************  FUNCTION  *********************/
template <>
void OptionDef<int>::load(dictionary* dic)
{
	char buffer[1024] = {'\0'};
	strcat(buffer,this->section);
	strcat(buffer,":");
	strcat(buffer,this->name);
	*(this->ptr)  = iniparser_getint(dic,buffer,*this->ptr);
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
	
	this->loadFromIniDic(iniDic);
	
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

/*******************  FUNCTION  *********************/
/**
 * Internal function to load options from iniDic.
**/
void Options::loadFromIniDic ( dictionary* iniDic )
{
	//errors
	assert(iniDic != NULL);
	
	for (int i = 0 ; i < options.size() ; i++)
		options[i]->load(iniDic);
}

/*******************  FUNCTION  *********************/
/**
 * Load values from string, mostly to be used from MATT_OPTION environment variable.
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
OptionStackMode Options::getStackMode ( void )
{
	if (this->stackMode == "backtrace")
	{
		return MATT_STACK_MAP_BACKTRACE;
	} else if (this->stackMode == "tree-enter-exit") {
		return MATT_STACK_TREE_ENTER_EXIT;
	} else if (this->stackMode == "map-enter-exit") {
		return MATT_STACK_MAP_ENTER_EXIT;
	} else {
		MATT_FATAL_ARG("Invalid stack mode : %1 !").arg(this->stackMode).end();
		return MATT_STACK_MAP_BACKTRACE;
	}
}

/*******************  FUNCTION  *********************/
/**
 * Need to be call once after malloc is available.
**/
Options& initGlobalOptions ( void )
{
	//error
	assume (gblOptions == NULL,"initGlobalOptions was used previously, gblOptions is already init ! ");
	void * ptr = MATT_NO_FREE_MALLOC(sizeof(Options));
	gblOptions = new(ptr) Options();
	return *gblOptions;
}

}

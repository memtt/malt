/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/common/OptionsMeta.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2026
***********************************************************/

/**********************************************************/
//std
#include <cstdio>
#include <cassert>
//internals
#include "Debug.hpp"
#include "OptionsMeta.hpp"
#include <json/JsonState.h>

/**********************************************************/
namespace MALT 
{

/**********************************************************/
OptionMetaBase::OptionMetaBase(const std::string & group, const std::string & key)
{
	this->group = group;
	this->key = key;
}

/**********************************************************/
OptionMetaBase::~OptionMetaBase(void)
{
}

/**********************************************************/
OptionMetaBase & OptionMetaBase::setDoc(const std::string & value)
{
	this->doc = value;
	return *this;
}

/**********************************************************/
const std::string & OptionMetaBase::getGroup(void) const
{
	return this->group;
}

/**********************************************************/
const std::string & OptionMetaBase::getKey(void) const
{
	return this->key;
}

/**********************************************************/
const std::string & OptionMetaBase::getDoc(void) const
{
	return this->doc;
}

/**********************************************************/
std::string OptionMetaBase::getGroupKey(void) const
{
	char key[4096];
	snprintf(key, sizeof(key), "%s:%s", this->getGroup().c_str(), this->getKey().c_str());
	return key;
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

}

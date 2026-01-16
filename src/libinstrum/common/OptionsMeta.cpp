/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 10/2025
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
}

/**********************************************************/
OptionMetaBase & OptionMetaBase::setDefault(const std::string & value)
{
	this->defaultValue = value;
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
const std::string & OptionMetaBase::getDefaultValue(void) const
{
	return this->defaultValue;
}

/**********************************************************/
const std::string & OptionMetaBase::getGroupKey(void) const
{
	return this->group + std::string(":") + this->key;
}

}
